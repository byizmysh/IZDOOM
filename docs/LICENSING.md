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

The source license is not a license to redistribute DOOM game data. The local
demonstration module embeds DOOM1.WAD plus generated textures and other derived game data.
That local module is not the public GitHub release asset prepared here.

The public source archives exclude IWADs, extracted assets, captures, memory
images and playable asset-bearing modules. `build.py --wad ...` reconstructs
the module from game data supplied by its user. No assumption is made here
that extracting shareware resources and repacking them into another module
is allowed under the original shareware distribution terms.

## Name and artwork

The release identifies itself as an unofficial DOOM port, credits id Software
and PureDOOM, and does not claim an original game, endorsement or trademark
ownership. The icon is new SVG artwork with an IZMYSH credit. No official
DOOM logo image, game screenshot or commercial art is copied into the icon.

This is an evidence-based release review, not a claim that the original game
assets or trademarks have become freely licensed.
