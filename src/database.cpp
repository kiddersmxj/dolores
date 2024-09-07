#include "../inc/database.hpp"

void PrependDebugFile(const std::string& text) {
    // Read the existing content of the file
    std::ifstream debugFileIn("debug");
    std::stringstream buffer;
    buffer << debugFileIn.rdbuf();
    std::string oldContent = buffer.str();
    debugFileIn.close();

    // Open the file in truncate mode to overwrite it
    std::ofstream debugFileOut("debug", std::ios::trunc);

    // Check if the file is open
    if (debugFileOut.is_open()) {
        // Write the new text followed by the old content
        debugFileOut << text << std::endl << oldContent;

        // Close the file
        debugFileOut.close();
    } else {
        std::cerr << "Unable to open the file 'debug'" << std::endl;
    }
}


Database::Database() {
    // Check if the directory exists, and create it if it doesn't
    try {
        if (!fs::exists(ChatArchiveDir)) {
            if (fs::create_directories(ChatArchiveDir)) {
                // std::cout << "Directory created: " << ChatArchiveDir << std::endl;
            } else {
                std::cerr << "Failed to create directory: " << ChatArchiveDir << std::endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    Get();
}

Database::~Database() {
}

void Database::Get() {
    jsonNames.clear();
    jsonFileNames.clear();

    // Vector to store files and their last modified time
    std::vector<FileData> files;

    // Iterate through the directory and collect .json file names
    try {
        for (const auto& entry : fs::directory_iterator(ChatArchiveDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                FileData fileData;
                fileData.filePath = entry.path();
                // Check if the file is "New Chat" or "0000000000.json"
                if (fileData.filePath.stem() == "New Chat" || fileData.filePath.stem() == "0000000000") {
                    // Assign current time to ensure it is placed at the beginning
                    fileData.lastModified = std::chrono::system_clock::now().time_since_epoch().count();
                } else {
                    // Use the actual last modified time
                    fileData.lastModified = fs::last_write_time(entry).time_since_epoch().count();
                }
                files.push_back(fileData);
            }
        }

        // Sort files by last modified time (most recent first)
        std::sort(files.begin(), files.end(), compareByModifiedTime);

        // Process the sorted files
        for (const auto& fileData : files) {
            jsonFileNames.push_back(fileData.filePath.stem().string());
            std::string Name = "";

            // Open the JSON file and parse it
            std::ifstream file(fileData.filePath);
            if (file.is_open()) {
                json j;
                file >> j;

                // Check if the "Name" key exists and add its value to the jsonNames vector
                for (const auto& item : j) {
                    if (item.is_array() && !item.empty() && item[0].is_string() && item[0] == "Name") {
                        Name = item[1].get<std::string>();
                        jsonNames.push_back(Name);
                    }
                }
            }
            // std::cout << fileData.filePath.filename().string() << " - " << Name << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << std::endl;
    }
}

// Function to generate a random 10-digit UID as a string
std::string Database::generateUID() {
    std::random_device rd;  // Seed for the random number engine
    std::mt19937_64 gen(rd()); // Use a 64-bit Mersenne Twister engine
    std::uniform_int_distribution<long long> dis(UIDRANGE); // Range for 10 digits
    return std::to_string(dis(gen));
}

void Database::SaveFile(const json& request_payload, const std::string& dir, std::string Name) {
    SaveFile(request_payload, dir, generateUID(), Name);
}

void Database::SaveFile(const json& request_payload, const std::string& dir, const std::string& uid, std::string Name) {
    // Ensure the directory exists
    struct stat info;
    if (stat(dir.c_str(), &info) != 0) {
        // Directory does not exist, so create it
        if (mkdir(dir.c_str(), 0777) != 0) {
            std::cerr << "Error: Failed to create directory " << dir << std::endl;
            return;
        }
    }

    std::string filename = dir + "/" + uid + ".json";

    json NamedFile = {
        {"Name", Name},
        {"UID", uid},
        {request_payload}
    };

    std::ofstream file(filename); // Open file for writing (will overwrite if it exists)
    if (file.is_open()) {
        file << NamedFile.dump(4); // Pretty-print JSON with an indent of 4 spaces
        file.close();
        // std::cout << "Saved conversation to " << filename << " - " << Name << std::endl;
    } else {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
    }
}

// json Database::ReadFile(std::string Name) {
//     PrependDebugFile(Name);
//     // Find the index of the Name in jsonNames
//     auto it = std::find(jsonNames.begin(), jsonNames.end(), Name);
//     if (it == jsonNames.end()) {
//         throw std::runtime_error("Name not found in jsonNames");
//     }

//     // Get the index
//     size_t index = std::distance(jsonNames.begin(), it);

//     // Use the index to find the corresponding file name in jsonFileNames
//     std::string fileName = ChatArchiveDir + jsonFileNames[index] + ".json";

//     // Read the file content
//     std::ifstream fileStream(fileName);
//     if (!fileStream.is_open()) {
//         throw std::runtime_error("Unable to open file: " + fileName);
//     }

//     // Parse the JSON content
//     json fileContent;
//     fileStream >> fileContent;

//     // Close the file stream
//     fileStream.close();

//     // Extract the "messages" part from the JSON structure
//     if (fileContent.is_array() && fileContent.size() >= 3) {
//         // The messages are located in the third element (index 2)
//         const json& thirdElement = fileContent[2];
//         if (thirdElement.is_object() && thirdElement.contains("messages")) {
//             return fileContent[2][0];
//             // return thirdElement["messages"];
//         } else {
//             throw std::runtime_error("'messages' not found in the expected structure");
//         }
//     } else {
//         throw std::runtime_error("Invalid JSON structure or insufficient elements");
//     }
// }

json Database::ReadFile(int Index) {
    // Check if the index is within the bounds of jsonFileNames
    if (Index < 0 || Index >= jsonFileNames.size()) {
        throw std::out_of_range("Index out of range");
    }

    // Get the file name using the index
    std::string fileName = ChatArchiveDir + jsonFileNames[Index] + ".json";

    // Read the file content
    std::ifstream fileStream(fileName);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    // Parse the JSON content
    json fileContent;
    fileStream >> fileContent;

    // Close the file stream
    fileStream.close();

    // Extract the "messages" part from the JSON structure
    if (fileContent.is_array() && fileContent.size() >= 3) {
        // The third element (index 2) is expected to be a JSON object
        const json& thirdElement = fileContent[2];
        if (thirdElement.is_object()) {
            // Check if the object contains the "messages" key
            if (thirdElement.contains("messages")) {
                return thirdElement;
            } else {
                throw std::runtime_error("'messages' not found in the third element");
            }
        } else if (thirdElement.is_array() && thirdElement.size() > 0 && thirdElement[0].is_object() && thirdElement[0].contains("messages")) {
            // If the third element is an array and the first element of that array contains the "messages" key
            return thirdElement[0];
        } else {
            throw std::runtime_error("The third element is not in the expected format");
        }
    } else {
        throw std::runtime_error("Invalid JSON structure or insufficient elements");
    }
}

std::vector<std::string> Database::GetFileNames() {
    return jsonFileNames;
}

std::vector<std::string> Database::GetNames() {
    return jsonNames;
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

