namespace strings {
    namespace errors {
        constexpr const char* INVALID_ARGUMENT = "Invalid argument";
        constexpr const char* INVALIID_SUBCOMMAND = "Invalid subcommand";
        constexpr const char* NOT_IN_PROJECT = "Not in a project directory";
        constexpr const char* PATH_NOT_EXIST = "Error: Path {} does not exist";
        constexpr const char* PATH_NOT_DIRECTORY = "Error: Path {} is not a directory";
        constexpr const char* PATH_NOT_EMPTY = "Error: Directory {} is not empty";
        constexpr const char* CANNOT_WRITE = "Error: Cannot write to directory {}";    
        constexpr const char* SPRITE_EXISTS = "Error: Sprite {} already exists";
        constexpr const char* SPRITE_DOES_NOT_EXIST = "Error: Sprite {} does not exist";    
        constexpr const char* CANNOT_CREATE_FILE = "Error: Cannot create file {}";
        constexpr const char* CANNOT_OPEN_FILE = "Error: Cannot open file {}";
    }
    namespace generic {
        constexpr const char* VERSION_STRING="version {:s}, compiled {:s} {:s} with {:s} {:s} for {:s} ({:s})";
    }
} // namespace strings