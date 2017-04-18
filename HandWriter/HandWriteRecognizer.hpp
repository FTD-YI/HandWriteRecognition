//
//  HandWriteRecognizer.hpp
//  HandWriter
//
//  Created by yi on 20/12/2016.
//  Copyright © 2016 yi. All rights reserved.
//

#ifndef HandWriteRecognizer_hpp
#define HandWriteRecognizer_hpp

#include <stdio.h>
#include <string>
#include <vector>

namespace HandWriteRecognizer {
    
    struct Point{
        double x;
        double y;
        double direction;
    };
    
    struct Stroke{
        std::vector<Point> points;
    };
    
    struct Word{
        std::string word;
        double dist;
        int strokeCount;
    };
    
    class Character{
        
    public:
        Character();
        void initSize(int width, int height);
        bool addPoint(int strokeId, double x, double y);
        void clear();
        
        int lastStrokeId;
        int strokeCount;
        int width, height;
        std::string word;
        std::vector<Stroke> strokes;
    };
    
    class Recognizer{
        
    public:
        Recognizer();
        bool loadModelFile(const char* filePath);
        bool recognize(Character &ch, std::vector<std::string> &resultWords, int count);
        
    private:
        std::vector<Character> characters;
        
    };
    
}

#endif /* HandWriteRecognizer_hpp */
