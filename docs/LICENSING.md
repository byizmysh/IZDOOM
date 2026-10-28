# Release licensing review — 2026-10-28

## What IZDOOM is

Inspection of the shipped guest confirms that it still compiles substantial
DOOM/PureDOOM source, including game rules, thinker/AI behavior, weapons,
collision, level loading, menus and intermission logic. Replacing the GPU
renderer  does not make those retained portions a
clean-room implementation. The release is described as a DOOM-derived port.

## Source license evidence

The exact pinned PureDOOM commit ships a GNU GPL version 2 license file:
https://github.com/Daivuk/PureDOOM/blob/355cfbd16fac119718879239336ee2ea408886bd/LICENSE

The upstream header also retains an older DOOM Source Code License notice.
That notice is preserved, rather than deleted or silently rewritten. The
official id Software repository identifies its source as GPL 2.0:
https://github.com/id-Software/DOOM/blob/master/LICENSE.TXT
https://github.com/id-Software/DOOM/blob/master/README.TXT

The combined release uses GPL version 2. Local contributions retain their
broader GPL-2.0-or-later grant, and explicitly MIT SDK headers keep MIT notices.
The complete corresponding sources and scripts are supplied in this archive.
Upstream references and downloaded-license hashes are recorded locally.

## Fork or standalone repository

GPL redistribution conditions concern source, notices and licensing. They do
not require GitHub's Fork UI feature. GitHub describes forks as repositories
connected to an upstream repository:
https://docs.github.com/en/pull-requests/reference/forks

Recommendation: a standalone **IZDOOM** repository, with the pinned PureDOOM
vendor file, explicit attribution, patch and complete build sources. This
fits the substantially different shader/guest layout. A GitHub fork of
PureDOOM is also possible, but would not make the source obligations disappear.

## IWAD and release artifacts

The source license does not grant rights over DOOM game data. The source
archives exclude the IWAD and derived runtime memory/resources. The separate
prebuilt IZ2S module contains DOOM shareware v1.9 episode one, not a retail IWAD.

The supplied 4,196,020-byte IWAD has MD5 f0cefca49926d00903cf57551d901abe
and SHA-256 1d7d43be501e67d927e415e0b8f3e29c3bf33075e859721816f652a526cac771.
Its original bytes are preserved both in the guest memory image and as
`game-data/DOOM1.WAD` for rebuilding. Generated indexed resources are runtime
representations of the same first-episode game content.

The Debian copyright record for doom-wad-shareware retains the id Software
license and John Carmack's 1999 clarification allowing distribution of the
DOOM shareware WAD:
https://sources.debian.org/src/doom-wad-shareware/1.9.fixed-4/debian/copyright/

That record is included verbatim as LICENSES/DOOM1-SHAREWARE.txt, with its
source and hash in docs/shareware-data.json. Game data keeps its own terms;
this is not a GPL grant over maps, textures, sprites, sounds or trademarks.
The prebuilt download is free of charge and includes no retail game data.

## Name and artwork

The release identifies itself as an unofficial DOOM port, credits id Software
and PureDOOM, and does not claim an original game, endorsement or trademark
ownership. The icon is new SVG artwork with an IZMYSH credit. No official
DOOM logo image, game screenshot or commercial art is copied into the icon.

This is an evidence-based release review, not a claim that the original game
assets or trademarks have become freely licensed.
