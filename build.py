#!/usr/bin/env python3
"""Build IZDOOM from complete sources and bundled episode-one game data.

No network, console connection, native caller or launcher bootstrap is used.
The resulting module embeds derived game data: keep it local unless you have
separate redistribution permission for that data.
"""
from pathlib import Path
import argparse,hashlib,json,shutil,struct,subprocess,sys,zipfile,zlib

ROOT=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'tools'))
from assets import build as build_assets,BASE as ASSET_BASE,Wad

def run(args):subprocess.run([str(v) for v in args],check=True)
def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--wad',type=Path,default=ROOT/'game-data/DOOM1.WAD',help='Original DOOM1.WAD; defaults to the bundled shareware episode')
    parser.add_argument('--output',type=Path,default=ROOT/'build')
    args=parser.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    wad=args.wad.read_bytes()
    if len(wad)>0x500000 or wad[:4]!=b'IWAD':raise SystemExit('This release requires an IWAD of at most 5 MiB (tested with DOOM1.WAD).')
    parsed=Wad(wad)
    for name in ['E1M1','PLAYPAL','COLORMAP','TEXTURE1','PNAMES']:
        if name not in parsed.names:raise SystemExit('Required lump missing: '+name)
    vendor=json.loads((ROOT/'vendor/upstream.json').read_text())
    assert sha((ROOT/'vendor/PureDOOM.h').read_bytes())==vendor['sha256'],'Pinned upstream header differs'
    elf=out/'izdoom-rv32.elf';binary=out/'izdoom-rv32.bin'
    run(['clang','--target=riscv32-unknown-elf','-march=rv32im','-mabi=ilp32','-O3',
         '-fno-builtin','-fno-strict-aliasing','-w','-std=c99','-ffreestanding',
         '-fno-stack-protector','-fno-pic','-ffunction-sections','-fdata-sections',
         '-DDOOM_LITE_ZEROED_HEAP=1','-nostdlib','-fuse-ld=lld',
         '-Wl,--gc-sections,-T,'+str(ROOT/'src/rv.ld'),'-I',ROOT/'src',
         ROOT/'src/doom_lite.c',ROOT/'src/rv-entry.c','-o',elf])
    run(['llvm-objcopy','-O','binary',elf,binary]);guest=binary.read_bytes()
    assert len(guest)<0xf0000,'Guest overlaps IWAD'
    memory=bytearray(ASSET_BASE)
    for address,value in [(0,0x52563332),(4,0x10000),(24,0x5fff00),(164,len(wad))]:struct.pack_into('<I',memory,address,value)
    memory[0x10000:0x10000+len(guest)]=guest
    memory[0x100000:0x100000+len(wad)]=wad
    header=(ROOT/'engine/PureDOOM-izdoom.h').read_text()
    resources,assets=build_assets(memory,header)
    memory.extend(resources)
    assert memory[ASSET_BASE:]==resources and struct.unpack_from('<I',memory,0x1d8)[0]==0
    package=out/'module';sw=package/'switch'
    sw.mkdir(parents=True,exist_ok=True)
    for name in ['app.vert','app.frag','app.comp','begin.comp','after.comp']:
        run(['glslangValidator','-V',ROOT/'shaders'/name,'-o',sw/(name+'.spv')])
        run(['spirv-val',sw/(name+'.spv')])
    for name in ['app.json','state.bin','data.bin']:shutil.copy2(ROOT/'assets'/name,sw/name)
    shutil.copy2(ROOT/'assets/izdoom-icon.png',sw/'icon.png')
    (sw/'memory.bin.deflate').write_bytes(zlib.compress(memory,6))
    game_data=package/'game-data';game_data.mkdir(exist_ok=True)
    (game_data/'DOOM1.WAD').write_bytes(wad)
    for name in ['module.json','LICENSE','COPYRIGHT','NOTICE.md','LICENSE-SCOPE.md','THIRD_PARTY.json','README.md','INSTALL-IZ2S.md','BUILD-FROM-SOURCE.md']:
        shutil.copy2(ROOT/name,package/name)
    shutil.copytree(ROOT/'LICENSES',package/'LICENSES',dirs_exist_ok=True)
    entries={p.relative_to(package).as_posix():p.read_bytes() for p in sorted(package.rglob('*'))if p.is_file()and p.name!='checksums.json'}
    checks={'algorithm':'sha256','files':{name:sha(data)for name,data in entries.items()}}
    entries['checksums.json']=(json.dumps(checks,indent=2)+'\n').encode()
    archive=out/'izdoom.izm'
    with zipfile.ZipFile(archive,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=6)as z:
        for name,data in entries.items():
            info=zipfile.ZipInfo(name,(2026,10,28,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o100644<<16;z.writestr(info,data)
    report={'built':True,'consoleExecuted':False,'id':'izdoom','version':'0.17.1',
        'archive':str(archive),'sha256':sha(archive.read_bytes()),'guestBytes':len(guest),
        'memoryBytes':len(memory),'assetBytes':len(resources),'wadSha256':sha(wad),
        'manualControls':True,'sourceVersion':'preserved 0.17, accepted 80% stick and crosshair removal',
        'distribution':'Source and prebuilt releases include original DOOM shareware v1.9 under its separate terms.'}
    (out/'build-report.json').write_text(json.dumps(report,indent=2)+'\n')
    (out/'assets.json').write_text(json.dumps(assets,indent=2)+'\n')
    print(json.dumps(report,indent=2))

if __name__=='__main__':main()
