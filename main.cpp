#include "FretBoard.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// Simple helper to parse scale arguments like "C#Maj" or "Am"
struct ScaleParser {
    static std::pair<NoteName, std::string> parse(std::string input) {
        static std::map<std::string, NoteName> nameToNote = {
            {"C", NoteName::C}, {"C#", NoteName::CSharp}, {"Db", NoteName::Db},
            {"D", NoteName::D}, {"D#", NoteName::DSharp}, {"Eb", NoteName::Eb},
            {"E", NoteName::E}, {"F", NoteName::F}, {"F#", NoteName::FSharp},
            {"Gb", NoteName::Gb}, {"G", NoteName::G}, {"G#", NoteName::GSharp},
            {"Ab", NoteName::Ab}, {"A", NoteName::A}, {"A#", NoteName::ASharp},
            {"Bb", NoteName::Bb}, {"B", NoteName::B}
        };

        NoteName root = NoteName::C;
        std::string type = "maj";
        bool found = false;

        if (input.length() >= 2) {
            std::string rootPart = input.substr(0, 2);
            if (nameToNote.count(rootPart)) {
                root = nameToNote[rootPart];
                type = input.substr(2);
                found = true;
            }
        }

        if (!found && input.length() >= 1) {
            std::string rootPart = input.substr(0, 1);
            if (nameToNote.count(rootPart)) {
                root = nameToNote[rootPart];
                type = input.substr(1);
                found = true;
            }
        }

        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
        if (type == "m" || type == "min" || type == "minor") type = "min";
        else type = "maj";

        return {root, type};
    }

    static std::vector<NoteName> getScaleNotes(NoteName root, std::string type) {
        std::vector<int> intervals;
        if (type == "min") {
            intervals = {0, 2, 3, 5, 7, 8, 10}; 
        } else {
            intervals = {0, 2, 4, 5, 7, 9, 11}; 
        }

        std::vector<NoteName> scale;
        for (int interval : intervals) {
            int noteVal = (static_cast<int>(root) + interval) % 12;
            scale.push_back(static_cast<NoteName>(noteVal));
        }
        return scale;
    }
};

void printHelp(const char* progName) {
    std::cout << "Fretboard Visualizer\n"
              << "Usage: " << progName << " [options] [scale]\n\n"
              << "Options:\n"
              << "  -h, --help     Show this help message\n"
              << "  -s, --sharps   Show all accidentals (sharps/flats) by default\n\n"
              << "Scales:\n"
              << "  Highlight a scale in Bold Red by passing its name.\n"
              << "  Format: [Root][Type] (e.g., C#Maj, Am, GMinor)\n"
              << "  - Major types: Maj, Major (default)\n"
              << "  - Minor types: m, min, minor\n\n"
              << "Examples:\n"
              << "  " << progName << " C#Maj\n"
              << "  " << progName << " --sharps Am\n"
              << "  " << progName << " -s\n";
}

int main(int argc, char *argv[])
{
    std::vector<std::pair<NoteName, int>> standardTuning = {
        {NoteName::E, 4}, {NoteName::B, 3}, {NoteName::G, 3}, 
        {NoteName::D, 3}, {NoteName::A, 2}, {NoteName::E, 2}
    };

    FretBoard fr(24, standardTuning);
    bool showSharps = false;
    std::string scaleName = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            return 0;
        } else if (arg == "--sharps" || arg == "-s") {
            showSharps = true;
        } else {
            scaleName = arg;
        }
    }

    std::cout << "Fretboard Visualizer (Standard E Tuning)\n";
    
    if (!scaleName.empty()) {
        auto [root, type] = ScaleParser::parse(scaleName);
        auto notes = ScaleParser::getScaleNotes(root, type);
        fr.setHighlightScale(notes);
        std::cout << "Highlighted Scale: " << Note::getName(root) << " " 
                  << (type == "min" ? "Minor" : "Major") << " (RED)\n";
    }
    
    std::cout << "------------------------------------------------------\n";
    
    fr.render(showSharps || !scaleName.empty(), 0, 12);
    std::cout << "\n";
    fr.render(showSharps || !scaleName.empty(), 13, 24);

    return 0;
}
