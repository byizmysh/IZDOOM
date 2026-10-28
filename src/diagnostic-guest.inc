
 {volatile uint32_t*d=(volatile uint32_t*)0x600;player_t*p=&players[consoleplayer];mobj_t*m=p->mo;
 d[0]=output.number;d[1]=gametic;d[2]=m?m->x:0;d[3]=m?m->y:0;d[4]=m?m->z:0;d[5]=m?m->angle:0;
 d[6]=viewx;d[7]=viewy;d[8]=viewz;
 sector_t*sec=m&&m->subsector?m->subsector->sector:0;
 d[9]=(uint32_t)(uintptr_t)sec;d[10]=menuactive;d[11]=paused;d[12]=gamestate;d[13]=automapactive;
 d[14]=sec?sec->floorheight:0;d[15]=sec?sec->ceilingheight:0;d[16]=buttons;d[17]=leveltime;d[18]=gamemap;d[19]=gameepisode;}
