# Attribution and provenance

- **DOOM**: original engine by id Software; original copyright notices remain
  in both engine headers. Official upstream distributes the source under GNU
  GPL 2.0: https://github.com/id-Software/DOOM/blob/master/LICENSE.TXT
- **PureDOOM**: single-header DOOM source port by Daivuk and contributors,
  https://github.com/Daivuk/PureDOOM, commit
  `355cfbd16fac119718879239336ee2ea408886bd`. The exact upstream header is
  `vendor/PureDOOM.h`; its SHA-256 is recorded in `vendor/upstream.json`.
  The GPL license from that exact commit is retained in `LICENSES`.
- **IZDOOM**: GPU RV32 integration, parallel world renderer, typed raster
  command path, fixed-rate scheduler, movement fixes,
  asset preparation, packaging and artwork by IZMYSH contributors, 2026.
  These modifications are visibly identified as modifications; they are not
  represented as original id Software/PureDOOM behavior.
- **IZ2W SDK interface headers**: copyright 2026 IZMYSH contributors; MIT
  permission and disclaimer are retained in `LICENSES/IZ2W-SDK-MIT.txt`.
- **Game data**: IWAD resources are owned/licensed separately by their owners.
  The source archive and prebuilt module preserve the original DOOM shareware v1.9 IWAD, with its separate license and redistribution clarification in `LICENSES/DOOM1-SHAREWARE.txt`. New icon artwork is
  original vector work and does not contain official DOOM artwork.

The historical DOOM Source Code License notice still appears in the upstream
header. It is preserved verbatim, alongside the GPL license file carried by
the pinned PureDOOM repository and the official id Software GPL release.
No proprietary game-data permission is inferred from either source license.

The IZ2W launcher is a separately installed platform dependency. This
module has no BlitBun dependency or host audio service. This source distribution
does not include a launcher, bootstrap or console firmware/SDK binary.
