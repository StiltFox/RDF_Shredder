# Stilt Fox™ RDF Shredder

This library is designed to read RDF files and use them to convert sql data into json-ld format. This library is experimental by nature, so use at your own risk. Tesing has not yet been implemented (And we would like to get this working soon). This was and still is a weekend project, so quality may not be up to snuff with other Stilt Fox™ projects.

Stilt Fox™ is not liable for any damages done to your hardware. For more information see LICENSE file.

Stilt Fox™ is trademarked. You may not use the Stilt Fox™ name, however this code is free to reference and use.

You may contribute to this library, however all contributions will share the same license as this library and you agree that Stilt Fox™ owns the copyright for any contributions.

## Considerations and Breif Mentions
### Database Compatibility
Right now only sqlite is officially supported by the Stilt Fox™ Universal library, however any database can be supported as long as you inheret the StiltFox::UniversalLibrary::DatabaseConnection class. This is an abstract class and you will need to implement all of the methods for your database of choice.

### Multi-Threading
Right now the multi threading capabilities of this library are very crude. Future support for controlling the number of threads and paging the sql queries is planned for future releases. This will require some updates to the underlying UiniversalLibrary as well. However, right now it's good enough to run so let's kick back and enjoy some iterative code design.

### Memory Management
Stilt Fox™ RDF Shredder works on the intire dataset while it is in memory. This design decision was made with cloud computing in mind. Saving to file after a query would hurt performance when inverse properties are bing updated; this is because it would require extra disk IO to save, then re open each previous reccord every time it gets referenced.

## Pre Requisites
- CMake must be installed on the system
    - Must be version 3.0.0 or higher
- Stilt Fox™ Universal Library
    - Can be installed by downloading from [here](https://github.com/StiltFox/StiltFox-Universal-Library).
- Json for Modern C++
    - Download it [here](https://github.com/nlohmann/json).
- GTest
    - This is only to run the tests. If you do not care about this and just want ot use the library, you can skip this.
    - [source](https://github.com/google/googletest)