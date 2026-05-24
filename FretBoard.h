#ifndef FRETBOARD_H
#define FRETBOARD_H

#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <set>

enum class NoteName {
    C = 0, CSharp, D, DSharp, E, F, FSharp, G, GSharp, A, ASharp, B,
    Db = 1, Eb = 3, Gb = 6, Ab = 8, Bb = 10 // Enharmonic aliases
};

struct Note {
    NoteName name;
    int octave;
    double frequency;

    Note(NoteName n, int oct);
    
    static double calculateFrequency(NoteName n, int oct);
    static std::string getName(NoteName n, bool useSharps = true);
    
    std::string toString(bool useSharps = true, bool includeOctave = true) const;
    bool isAccidental() const;
};

class SteelString {
    Note rootNote;
public:
    SteelString(NoteName rn, int oct) : rootNote(rn, oct) {}
    Note getNoteAtFret(int fret) const;
    const Note& getRootNote() const { return rootNote; }
};

class FretBoard {
    int nFrets;
    std::vector<SteelString> strings;
    bool useSharps;
    std::set<NoteName> highlightedNotes;

public:
    FretBoard(int frets = 22, const std::vector<std::pair<NoteName, int>>& tuning = {
        {NoteName::E, 4}, {NoteName::B, 3}, {NoteName::G, 3}, 
        {NoteName::D, 3}, {NoteName::A, 2}, {NoteName::E, 2}
    });

    void setHighlightScale(const std::vector<NoteName>& scale);
    void clearHighlights();
    void render(bool showAccidentals = true, int startFret = 0, int endFret = -1);
};

#endif
