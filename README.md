![logo](logo.png "VVVVVV Modding Base")

This is a fork of [Terry Cavanagh](http://distractionware.com/)'s [VVVVVV](https://thelettervsixtim.es/), aimed to make custom campaigns easier.

This modding base is only intended for custom campaigns, NOT anything which aims to modify the main campaign. For example:

## Types of mods this is aimed to be used for:

- [VVVVVV: The Depths](https://nyakofox.itch.io/vvvvvv-the-depths)
- [VVVVVV 3.0](https://youtu.be/fTeoSF7QFeA) (Playthrough by mothbeanie)
- Veep's Venture ([showcase 1](https://youtu.be/MdzJX1YJ2iw), [showcase 2](https://youtu.be/ksx1zMxMalM))

## Types of mods this is NOT aimed to be used for:

- [VVVVVV: Chaos Edition](https://distractionware.com/forum/index.php?topic=4299.0) (Is intended for use with the main campaign)
- [cursed_vvvvvv](https://mustardbucket.itch.io/cursed-vvvvvv?secret=O0KvS02wD473pXBF9avreZsww) (Is intended for use with the main campaign)

## What's in the base?

This fork was primarily made for myself,
as doing the same code edits every time I want to start another mod like this is not only annoying,
but also prone to errors and mistakes.

Nonetheless, here are the features:

- JSON file to configure mods
- Github Actions for producing macOS app bundles
- Launches directly into a level from the `play` button on the menu
- Custom level additions used commonly by mods

## JSON format

(The comments are only here for descriptions, the base does not actually support JSON-C.

```jsonc
{
    "name": "VVVVVV: Modding Base", // The name of the mod, used in various places
    "version": "1.0", // The version of the mod
    "release_build": true, // Whether or not it's a release build or not

    "title": {
        "menu_version": "modding base v1.0", // The version tag in the bottom-right of the menu
        "logo_x": 67, // The x-coordinate of the logo on the menu
        "logo_y": 54, // The y-coordinate of the logo on the menu
        "edition_text": "MODDING BASE", // The "edition" text, can be an empty string
        "edition_text_x": 264, // The x-coordinate of the edition text
        "edition_text_y": 85 // The y-coordinate of the edition text
    },

    "level_filename": "testlevel", // The level file to load into (remove the .vvvvvv, and do not use zips)
    "allow_custom_levels": false, // Whether or not the custom levels menu is accessible or not

    "data_name": "data_modbase.zip", // The name of the data.zip to use
    "data_url": "https://example.com/data_modbase.zip", // Your modified copy of data.zip hosted somewhere

    // Apple-specific configuration
    "xcode": {
        "produce_app_bundle": false, // Whether or not a macOS app bundle is built or not

        // Make sure you're following Xcode's guidelines for the below options:
        "product_bundle_identifier": "gay.nyako.vvvvvvmodbase", // The identifier for the mod
        "product_bundle_identifier_mobile": "gay.nyako.vvvvvvmodbasemobile", // The identifier for the mod's mobile version
        "product_name": "VVVVVV Modding Base", // The product name, this'll be the exported filename (so no colons, ex. "VVVVVV: Modding Base")
        "project_version": "1.0", // The mod's "build" (must be int or decimal, no "1.0.0", and must only increase)
        "marketing_version": "1.0" // The mod's marketing version (what gets displayed to the user, typically major.minor.patch)
    }
}
```

## Misc. info

The original game was made by [Terry Cavanagh](http://distractionware.com/), with music by [Magnus Pålsson](http://souleye.madtracker.net/).

The source code for the desktop version is in [this folder](desktop_version).

If you'd like to talk about mods, custom levels, or VVVVVV in general, feel free to join the "unofficial" [VVVVVV Discord](https://discord.gg/Zf7Nzea)!

Modding Base Credits
-------
- Programming: [Alexandra Fox](https://nyako.dev/)
- macOS CI: [leo vriska](https://vriska.dev/)

Game Credits
-------
- Created by [Terry Cavanagh](http://distractionware.com/)
- Room Names by [Bennett Foddy](http://www.foddy.net)
- Music by [Magnus Pålsson](https://magnuspalsson.com/)
- Metal Soundtrack by [FamilyJules](https://link.space/@familyjules)
- 2.0 Update (C++ Port) by [Simon Roth](http://www.machinestudios.co.uk)
- 2.2 Update (SDL2/PhysicsFS/Steamworks port) by [Ethan Lee](http://www.flibitijibibo.com/)
- Additional coding by [Misa Kai](https://infoteddy.info/)
- Beta Testing by Sam Kaplan and Pauli Kohberger
- Ending Picture by Pauli Kohberger
- Localisations by [our localisation teams](desktop_version/TRANSLATORS.txt)
- With additional contributions by [many others here on github](desktop_version/CONTRIBUTORS.txt) <3
