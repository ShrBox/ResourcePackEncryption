# ResourcePackEncryption

Provide resource pack encryption and CDN support for LeviLamina.

## Encryption

You can encryption your resource packs on websites like [PackCrypt](https://endstone-insider.github.io/pack-crypt/)

## Example configuration file

plugins/ResourcePackEncryption/config/config.json
```jsonc
{
    "version": 1,
    "ResourcePacks": {
        "a0705267-a710-49b8-b4b3-554ea763b97d": "114514", // "uuid": "contentkey"
        "1f4ca43b-27d2-4f35-943d-cce6e374e8d9": "1919810"
    },
    "ResourcePacksCDN": {
        "a0705267-a710-49b8-b4b3-554ea763b97d": "http://127.0.0.1/a.mcpack", // "uuid": "url"
        "1f4ca43b-27d2-4f35-943d-cce6e374e8d9": "http://127.0.0.1/b.mcpack"
    }
}
```
