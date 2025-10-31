namespace strings {
    namespace errors {
        constexpr const char* invalid_argument = "Invalid argument";
        constexpr const char* invalid_subcommand = "Invalid subcommand";
        constexpr const char* not_in_project = "Not in a project directory";
        constexpr const char* path_not_exist = "Error: Path {} does not exist";
        constexpr const char* path_not_directory = "Error: Path {} is not a directory";
        constexpr const char* path_not_empty = "Error: Directory {} is not empty";
        constexpr const char* cannot_write = "Error: Cannot write to directory {}";    
        constexpr const char* sprite_exists = "Error: Sprite {} already exists";
        constexpr const char* sprite_does_not_exist = "Error: Sprite {} does not exist";    
        constexpr const char* cannot_create_file = "Error: Cannot create file {}";
        constexpr const char* cannot_open_file = "Error: Cannot open file {}";
    }
    namespace generic {
        constexpr const char* version_string="version {:s}, compiled {:s} {:s} with {:s} {:s} for {:s} ({:s})";
    }
}