#include "FretBoard.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>

// ANSI Color Codes
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string CYAN = "\033[36m";
const std::string YELLOW = "\033[33m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string MAGENTA = "\033[35m";

Note::Note(NoteName n, int oct) : name(n), octave(oct) {
    frequency = calculateFrequency(n, oct);
}

double Note::calculateFrequency(NoteName n, int oct) {
    int semitones = static_cast<int>(n) + (oct + 1) * 12;
    return 440.0 * std::pow(2.0, (semitones - 69) / 12.0);
}

std::string Note::getName(NoteName n, bool useSharps) {
    static const std::vector<std::string> sharps = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    static const std::vector<std::string> flats = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
    int idx = static_cast<int>(n) % 12;
    return useSharps ? sharps[idx] : flats[idx];
}

std::string Note::toString(bool useSharps, bool includeOctave) const {
    std::string s = getName(name, useSharps);
    if (includeOctave) s += std::to_string(octave);
    return s;
}

bool Note::isAccidental() const {
    int idx = static_cast<int>(name) % 12;
    return (idx == 1 || idx == 3 || idx == 6 || idx == 8 || idx == 10);
}

Note SteelString::getNoteAtFret(int fret) const {
    int rootIdx = static_cast<int>(rootNote.name);
    int totalSemitones = rootIdx + fret;
    int noteIdx = totalSemitones % 12;
    int octaveShift = (rootIdx + fret) / 12;
    return Note(static_cast<NoteName>(noteIdx), rootNote.octave + octaveShift);
}

FretBoard::FretBoard(int frets, const std::vector<std::pair<NoteName, int>>& tuning) 
    : nFrets(frets), useSharps(true) {
    for (const auto& t : tuning) {
        strings.emplace_back(t.first, t.second);
    }
}

void FretBoard::setHighlightScale(const std::vector<NoteName>& scale) {
    highlightedNotes.clear();
    for (auto n : scale) {
        highlightedNotes.insert(static_cast<NoteName>(static_cast<int>(n) % 12));
    }
}

void FretBoard::clearHighlights() {
    highlightedNotes.clear();
}

void FretBoard::render(bool showAccidentals, int startFret, int endFret) {
    // Normalize fret range: ensure endFret doesn't exceed board capacity
    if (endFret == -1 || endFret > nFrets) endFret = nFrets;
    
    const int CELL_WIDTH = 6;

    // 1. Header: Print fret numbers aligned with the fret cells
    std::cout << std::string(CELL_WIDTH, ' '); 
    for (int f = startFret; f <= endFret; ++f) {
        std::cout << std::setw(CELL_WIDTH) << f;
    }
    std::cout << "\n";

    // 2. Strings: Iterate through each guitar string from highest (E4) to lowest (E2)
    for (const auto& str : strings) {
        // Print the string's root note label (e.g., "E4")
        std::string label = str.getRootNote().toString(useSharps, true);
        std::cout << std::setw(CELL_WIDTH - 1) << label << "|";

        // Print each fret position for the current string
        for (int f = startFret; f <= endFret; ++f) {
            Note n = str.getNoteAtFret(f);
            bool accidental = n.isAccidental();
            std::string noteName = n.toString(useSharps, false);
            std::string display;

            // Check if this note is part of the currently highlighted scale
            bool isHighlighted = highlightedNotes.count(static_cast<NoteName>(static_cast<int>(n.name) % 12));

            // Determine what to display based on note type and visibility settings
            if (!accidental || showAccidentals || isHighlighted) {
                if (isHighlighted) display = RED + BOLD + noteName + RESET;           // Highlighted scale notes (Red)
                else if (accidental) display = YELLOW + noteName + RESET;            // Sharp/Flat notes (Yellow)
                else if (n.name == str.getRootNote().name) display = GREEN + BOLD + noteName + RESET; // Root/Octaves of open string (Green)
                else display = CYAN + noteName + RESET;                              // Natural notes (Cyan)
            } else {
                display = "---"; // Hide accidental if not requested or highlighted
                noteName = "---";
            }

            // Cell centering logic: padding the note name to fit the fret cell
            if (f == 0) {
                // Fret 0 (Nut) uses a double-pipe separator and space padding
                int padLeft = (4 - noteName.length()) / 2;
                int padRight = 4 - noteName.length() - padLeft;
                std::cout << std::string(padLeft, ' ') << display << std::string(padRight, ' ') << "||";
            } else {
                // Other frets use single pipe and dash padding to simulate string lines
                int padLeft = (5 - noteName.length()) / 2;
                int padRight = 5 - noteName.length() - padLeft;
                std::cout << std::string(padLeft, '-') << display << std::string(padRight, '-') << "|";
            }
        }
        std::cout << "\n";
    }

    // 3. Markers: Print position dots (single at 3,5,7,9... and double at 12,24)
    std::cout << std::string(CELL_WIDTH, ' ');
    for (int f = startFret; f <= endFret; ++f) {
        bool marker = (f == 3 || f == 5 || f == 7 || f == 9 || f == 15 || f == 17 || f == 19 || f == 21);
        bool doubleMarker = (f == 12 || f == 24);
        std::string sym = doubleMarker ? ":" : (marker ? "." : " ");
        
        if (f == 0) {
            std::cout << std::string(CELL_WIDTH, ' ');
        } else {
            // Center the marker under the fret cell
            int padLeft = (5 - sym.length()) / 2;
            int padRight = 5 - sym.length() - padLeft;
            std::cout << std::string(padLeft, ' ') << sym << std::string(padRight + 1, ' ');
        }
    }
    std::cout << "\n";
}
