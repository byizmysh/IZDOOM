"""Bounded offline WAD -> immutable indexed GPU resources. No console access."""
from pathlib import Path
import struct, re, hashlib, json

BASE = 0x1800000
END = 0x1f00000
MAGIC = 0x534c5431


class Wad:
    def __init__(self, data):
        self.data = data
        if len(data) < 12 or data[:4] not in (b'IWAD', b'PWAD'):
            raise ValueError('Invalid WAD header')
        count, directory = struct.unpack_from('<II', data, 4)
        if count > 65536 or directory + count * 16 > len(data):
            raise ValueError('Invalid WAD directory')
        self.lumps = []
        self.names = {}
        for i in range(count):
            at, n, name = struct.unpack_from('<II8s', data, directory + i * 16)
            if at + n > len(data):
                raise ValueError('Lump out of bounds')
            name = name.rstrip(b'\0').decode('ascii').upper()
            self.lumps.append((name, data[at:at+n]))
            self.names[name] = i

    def get(self, name):
        return self.lumps[self.names[name]][1]


def patch(data):
    """Decode Doom posts. Transparent texels use 256, preserving palette index 0."""
    if len(data) < 8:
        raise ValueError('Truncated patch')
    w, h, left, top = struct.unpack_from('<HHhh', data)
    if not 0 < w <= 4096 or not 0 < h <= 4096 or 8 + 4*w > len(data):
        raise ValueError('Invalid patch dimensions')
    pixels = [256] * (w*h)
    for x in range(w):
        pos, = struct.unpack_from('<I', data, 8 + x*4)
        previous = -1
        for _ in range(len(data)):
            if pos >= len(data):
                raise ValueError('Truncated patch column')
            delta = data[pos]
            if delta == 255:
                break
            if pos+3 > len(data):
                raise ValueError('Truncated patch post')
            n = data[pos+1]
            if pos+4+n > len(data):
                raise ValueError('Truncated patch pixels')
            # Relative top deltas also handle the tall-patch extension.
            y = delta + previous if delta <= previous else delta
            previous = y
            for j, value in enumerate(data[pos+3:pos+3+n]):
                if 0 <= y+j < h:
                    pixels[(y+j)*w+x] = value
            pos += n+4
        else:
            raise ValueError('Unterminated patch column')
    return w, h, left, top, pixels


class Pack:
    def __init__(self):
        self.data = bytearray(128)

    def reserve(self, n):
        self.data.extend(bytes((-len(self.data)) % 4))
        at = len(self.data)
        if BASE + at + n > END:
            raise ValueError('Immutable resources overlap display memory')
        self.data.extend(bytes(n))
        return BASE+at

    def put(self, data):
        at = self.reserve(len(data))
        self.data[at-BASE:at-BASE+len(data)] = data
        return at

    def words(self, at, *values):
        struct.pack_into('<'+'I'*len(values), self.data, at-BASE,
                         *(v & 0xffffffff for v in values))


def build(memory, header):
    wad_bytes, = struct.unpack_from('<I', memory, 164)
    wad = Wad(memory[0x100000:0x100000+wad_bytes])
    pack = Pack()
    patch_cache = {}
    decoded = {}

    def decode(i):
        if i not in decoded:
            decoded[i] = patch(wad.lumps[i][1])
        return decoded[i]

    def packed_patch(i):
        if i not in patch_cache:
            w, h, left, top, pixels = decode(i)
            texels = pack.put(struct.pack('<'+'H'*len(pixels), *pixels))
            meta = pack.reserve(20)
            pack.words(meta, w, h, left, top, texels)
            patch_cache[i] = meta
        return patch_cache[i]

    pnames = wad.get('PNAMES')
    pn, = struct.unpack_from('<I', pnames)
    if pn > 65536 or 4+pn*8 > len(pnames):
        raise ValueError('Invalid PNAMES')
    patch_names = [pnames[4+i*8:12+i*8].rstrip(b'\0').decode('ascii').upper() for i in range(pn)]
    textures = []
    for lump in ('TEXTURE1', 'TEXTURE2'):
        if lump not in wad.names:
            continue
        table = wad.get(lump)
        n, = struct.unpack_from('<I', table)
        if n > 65536 or 4+n*4 > len(table):
            raise ValueError('Invalid texture table')
        for i in range(n):
            at, = struct.unpack_from('<I', table, 4+i*4)
            if at+22 > len(table):
                raise ValueError('Truncated texture header')
            name = table[at:at+8].rstrip(b'\0').decode('ascii')
            w,h = struct.unpack_from('<HH', table, at+12)
            count, = struct.unpack_from('<H', table, at+20)
            if not 0 < w <= 4096 or not 0 < h <= 4096 or at+22+count*10 > len(table):
                raise ValueError('Invalid texture dimensions or patch count')
            pixels = [256]*(w*h)
            for j in range(count):
                ox,oy,pid = struct.unpack_from('<hhH', table, at+22+j*10)
                if pid >= pn:
                    raise ValueError('Invalid texture patch ID')
                pw,ph,_,_,src = decode(wad.names[patch_names[pid]])
                for sy in range(max(0,-oy), min(ph,h-oy)):
                    for sx in range(max(0,-ox), min(pw,w-ox)):
                        value=src[sy*pw+sx]
                        if value != 256:
                            pixels[(sy+oy)*w+sx+ox] = value
            texels=pack.put(struct.pack('<'+'H'*len(pixels),*pixels))
            textures.append((name,w,h,texels))
    texture_table=pack.reserve(len(textures)*16)
    for i,(_,w,h,texels) in enumerate(textures):
        pack.words(texture_table+i*16,w,h,0,texels)

    match=re.search(r'char\* sprnames\[NUMSPRITES \+ 1\]\s*=\s*\{(.*?)\};',header,re.S)
    if not match:
        raise ValueError('Exact sprite enum names missing')
    names=re.findall(r'"([A-Z0-9]{4})"',match[1])
    sprite_ids={name:i for i,name in enumerate(names)}
    sprite_table=pack.reserve(len(names)*32*8*4)
    sprite_entries=0
    first=wad.names['S_START']+1
    last=wad.names['S_END']
    for i in range(first,last):
        name=wad.lumps[i][0]
        if len(name)<6 or name[:4] not in sprite_ids:
            continue
        for k in (4,6):
            if len(name)<k+2:
                continue
            frame=ord(name[k])-65
            rotation=ord(name[k+1])-48
            if not 0<=frame<32 or not 0<=rotation<=8:
                raise ValueError('Invalid sprite frame/rotation')
            value=packed_patch(i)|(0x80000000 if k==6 else 0)
            for r in (range(8) if rotation==0 else [rotation-1]):
                pack.words(sprite_table+((sprite_ids[name[:4]]*32+frame)*8+r)*4,value)
                sprite_entries+=1
    cmap=wad.get('COLORMAP')
    if len(cmap)<32*256:
        raise ValueError('Missing lighting tables')
    colormap=pack.put(cmap[:32*256])
    digits=[packed_patch(wad.names['STTNUM'+str(i)]) for i in range(10)]
    first_flat,last_flat=wad.names['F_START']+1,wad.names['F_END']
    flat_addresses=[]
    flat_markers=[]
    for i in range(first_flat,last_flat):
        name,raw=wad.lumps[i]
        # R_InitFlats retains subnamespace markers in its lump-relative indices.
        # Keep those slots rather than shifting every actual sector flat ID.
        if not raw and re.fullmatch(r'F[0-9]+_(START|END)',name):
            flat_markers.append({'index':i-first_flat,'name':name})
            raw=bytes(4096)
        if len(raw)!=4096:
            raise ValueError('Flat must contain exactly 64x64 indexed texels: '+name)
        flat_addresses.append(pack.put(raw))
    flat_table=pack.reserve(len(flat_addresses)*4)
    pack.words(flat_table,*flat_addresses)
    sky_texture=next(i for i,t in enumerate(textures) if t[0].upper()=='SKY1')
    hud_names=['STBAR','STARMS','STTPRCNT','STTMINUS']
    hud_names+=['STYSNUM'+str(i) for i in range(10)]
    hud_names+=['STGNUM'+str(i) for i in range(2,8)]
    hud_names+=['STKEYS'+str(i) for i in range(6)]
    for pain in range(5):
        hud_names+=['STFST'+str(pain)+str(i) for i in range(3)]
        hud_names+=['STFTR'+str(pain)+'0','STFTL'+str(pain)+'0',
                    'STFOUCH'+str(pain),'STFEVL'+str(pain),'STFKILL'+str(pain)]
    hud_names+=['STFGOD0','STFDEAD0']
    hud_patches=[packed_patch(wad.names[name]) for name in hud_names]
    hud_table=pack.reserve(len(hud_patches)*4);pack.words(hud_table,*hud_patches)
    pack.words(BASE,MAGIC,1,len(pack.data),len(textures),texture_table,len(names),sprite_table,colormap,*digits)
    pack.words(BASE+72,len(flat_addresses),flat_table,sky_texture)
    pack.words(BASE+84,hud_table,len(hud_patches))
    return pack.data, {'format':1,'base':BASE,'end':BASE+len(pack.data),
        'bytes':len(pack.data),'wadSha256':hashlib.sha256(wad.data).hexdigest(),
        'textureCount':len(textures),'flatCount':len(flat_addresses),'flatMarkers':flat_markers,'skyTexture':sky_texture,'hudPatches':len(hud_patches),
        'spriteEnums':len(names),'spriteAssignments':sprite_entries,
        'decodedPatches':len(decoded),'packedPatches':len(patch_cache),
        'textures':[{'index':i,'name':v[0],'width':v[1],'height':v[2]} for i,v in enumerate(textures)]}


if __name__ == '__main__':
    import argparse
    parser=argparse.ArgumentParser()
    parser.add_argument('memory',type=Path)
    parser.add_argument('header',type=Path)
    parser.add_argument('output',type=Path)
    args=parser.parse_args()
    data,report=build(args.memory.read_bytes(),args.header.read_text())
    args.output.write_bytes(data)
    args.output.with_suffix('.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='textures'},indent=2))
