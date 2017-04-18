//
//  HandWriteRecognizer.cpp
//  HandWriter
//
//  Created by yi on 20/12/2016.
//  Copyright © 2016 yi. All rights reserved.
//

#include "HandWriteRecognizer.hpp"
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
#include <regex>

#define DEAFULT_WIDTH 1000
#define DEAFULT_HEIGHT 1000

#define MAX_DIFF_PER_STROKE 35
#define MAX_DIRETION 1000
#define VERTICAL 1001
#define HORIZONTAL 1002

using namespace HandWriteRecognizer;

double diretion(const Point &lastPoint, const Point &startPoint){
    double result = -1;
    result = atan2(startPoint.y - lastPoint.y,  startPoint.x - lastPoint.x) * 10;
    return result;
}


void norm(Character &character){
    Point lastPoint;
    lastPoint.x = -1;
    lastPoint.y = -1;
    for(int i = 0; i < character.strokeCount; ++i){
        Stroke stroke = character.strokes[i];
        Point tmpPoint;
        for(int j = 0; j < stroke.points.size(); ++j){
            tmpPoint = stroke.points[j];
            character.strokes[i].points[j].direction = (lastPoint.x == -1 && lastPoint.y == -1) ? 0 : diretion(lastPoint, tmpPoint);
            lastPoint = tmpPoint;
        }
    }
}

void turnPoints(Stroke *stroke, std::vector<Point> *points, int pointIndex1, int pointIndex2){
    if(pointIndex1 < 0 || pointIndex2 <= 0 || pointIndex1 >= pointIndex2 - 1)
        return;
    const float a = stroke->points[pointIndex2].x - stroke->points[pointIndex1].x;
    const float b = stroke->points[pointIndex2].y - stroke->points[pointIndex1].y;
    const float c = stroke->points[pointIndex1].x * stroke->points[pointIndex2].y - stroke->points[pointIndex2].x * stroke->points[pointIndex1].y;
    float max = 3000;
    int maxDistPointIndex = -1;
    for(int i = pointIndex1 + 1; i < pointIndex2; ++i){
        Point point = stroke->points[i];
        const float dist = fabs((a * point.y) -(b * point.x) + c);
        std::cout << dist << std::endl;
        if (dist > max) {
            max = dist;
            maxDistPointIndex = i;
        }
    }
    if(maxDistPointIndex != -1){
        turnPoints(stroke, points, pointIndex1, maxDistPointIndex);
        points->push_back(stroke->points[maxDistPointIndex]);
        turnPoints(stroke, points, maxDistPointIndex, pointIndex2);
    }
}

void getTurnPoints(Character &character){
    for(int i = 0; i < character.strokeCount; ++i){
        Stroke *stroke = &character.strokes[i];
        if(stroke->points.size() > 1){
            std::vector<Point> points;
            points.push_back(stroke->points[0]);
            turnPoints(stroke, &points, 0, (int)stroke->points.size() - 1);
            points.push_back(stroke->points[stroke->points.size() - 1]);
            stroke->points.clear();
            for(int i = 0; i < points.size(); ++i){
                stroke->points.push_back(points[i]);
            }
        }
    }
}


double distBetweenStrokes(const Stroke &stroke1, const Stroke &stroke2){
    double strokeDist = MAXFLOAT;
    std::cout << "Stroke size::" << stroke1.points.size() << " " << stroke2.points.size() << std::endl;
    double dist = 0.0f;
    int minLength = fmin(stroke1.points.size(), stroke2.points.size());
    Stroke largeStroke = stroke1.points.size() > minLength ? stroke1 : stroke2;
    Stroke smallStroke = stroke1.points.size() > minLength ? stroke2 : stroke1;
    for(int j = 1; j < minLength; ++j){
        double diretion1 = largeStroke.points[j].direction;
        double diretion2 = smallStroke.points[j].direction;
        // 垂直笔画处理
        if(diretion1 == VERTICAL && diretion2 == VERTICAL){
            dist += fabs(0.1);
            // 水平笔画处理
        }else if(fabs(diretion1) == HORIZONTAL && fabs(diretion2) == HORIZONTAL){
            dist += fabs(diretion1 - diretion2);
        }else{
            if(fabs(diretion1) == HORIZONTAL)
                diretion1 = 0.1f;
            if(fabs(diretion2) == HORIZONTAL)
                diretion2 = 0.1f;
            dist += fabs(diretion1 - diretion2);
        }
    }
    // 当前笔与上一笔的largeStroke位置
    dist += fabs(largeStroke.points[0].direction - smallStroke.points[0].direction);
    strokeDist = dist / minLength;
    std::cout << strokeDist << std::endl;
    return strokeDist;
}

double allMax = 0, allMin = MAXFLOAT;
double dist(const Character &character1, const Character &character2){
    double dist = MAXFLOAT;
    if(character2.strokeCount >= character1.strokeCount && character2.strokeCount <= character1.strokeCount + 2){
        std::cout << character1.word << ":"<< character1.strokeCount << "->" << character2.word << ":"<< character2.strokeCount << std::endl;
        double allStrokeDist = 0.0f;
        for(int i = 0; i < character1.strokeCount; ++i){
            Stroke stroke1 = character1.strokes[i];
            Stroke stroke2 = character2.strokes[i];
            double strokeDist = distBetweenStrokes(stroke1, stroke2);
            
            if(strokeDist > allMax)
                allMax = strokeDist;
            if(strokeDist < allMin)
                allMin = strokeDist;

            
            allStrokeDist += strokeDist;
            
            if(strokeDist > MAX_DIFF_PER_STROKE){
                allStrokeDist = MAXFLOAT;
                return allStrokeDist;
            }
        }
        // 笔画更接近的优先级更高
        return allStrokeDist / character1.strokeCount + character2.strokeCount - character1.strokeCount;
    }
    return dist;
}


static bool cmp_word_dist(const Word &word1, const Word &word2)
{
    return word1.dist < word2.dist;
}

Character::Character(){
    clear();
}

void Character::initSize(int tmpWidth, int tmpHeight){
    width = tmpWidth;
    height = tmpHeight;
}

bool Character::addPoint(int strokeId, double x, double y){
    if(strokeId < 0)
        return false;
    if(strokeId != lastStrokeId){
        lastStrokeId = strokeId;
        strokeCount++;
        Stroke stroke;
        strokes.push_back(stroke);
    }
    Point point;
    point.x = x / width * DEAFULT_WIDTH;
    point.y = y / height * DEAFULT_HEIGHT;
    strokes[strokeCount - 1].points.push_back(point);
    return true;
}

void Character::clear(){
    strokeCount = 0;
    lastStrokeId = -1;
    width = DEAFULT_WIDTH;
    height = DEAFULT_HEIGHT;
    strokes.clear();
}

Recognizer::Recognizer(){
    
}

bool Recognizer::loadModelFile(const char* filePath){
    char buffer[1024];
    std::ifstream ifs(filePath);
    if(!ifs.is_open()){
        std::cout << "Load model fail.." << std::endl;
        return false;
    }
    while(!ifs.eof())
    {
        ifs.getline(buffer, 1024);
        std::string line = buffer;
        if(line.length() > 0){
            Character character;
            character.strokeCount = 0;
            character.word = line.substr(0, 3);
            std::string strokes = line.substr(4, line.length() - 4);
            
            std::smatch match;
            std::regex expression("\\[.*?\\]");
            while(regex_search(strokes, match, expression)){
                std::string result = match[0];
                result.erase(result.length() - 1, 1);
                result.erase(0, 1);
                std::vector<std::string> directions;
                std::string delim = ",";
                size_t last = 0;
                size_t index = result.find_first_of(delim,last);
                while(index != std::string::npos)
                {
                    directions.push_back(result.substr(last, index-last));
                    last = index + 1;
                    index= result.find_first_of(delim,last);
                }
                if(index - last > 0)
                    directions.push_back(result.substr(last, index-last));
                Stroke stroke;
                for(int i = 0; i < directions.size(); ++i){
                    Point point;
                    point.direction = atof(directions[i].c_str());
                    stroke.points.push_back(point);
                }
                character.strokes.push_back(stroke);
                character.strokeCount++;
                if(strokes.length() > result.length())
                strokes.erase(strokes.begin(), strokes.begin() + result.length() + 2);
                else
                break;
            };
            characters.push_back(character);
        }
    }
    std::cout << "Load model finish.." << std::endl;
    return true;
}


bool Recognizer::recognize(Character &character, std::vector<std::string> &resultWords, int count)
{
    allMax = 0;
    allMin = MAXFLOAT;
    if(character.strokeCount == 0)
        return false;
    norm(character);
    getTurnPoints(character);
    std::vector<Word> words;
    for(int i = 0; i < characters.size(); ++i){
        Character tmpCharacter = characters[i];
        Word word;
        word.word = tmpCharacter.word;
        word.dist = dist(character, tmpCharacter);
        word.strokeCount = tmpCharacter.strokeCount;
        if(word.dist < 200){
            words.push_back(word);
        }
    }
    std::cout << allMax << " " << allMin << std::endl;
    std::sort(words.begin(), words.end(), cmp_word_dist);
    for(int i = 0; i < words.size() && i < count; ++i){
        Word word = words[i];
        printf("%s:%lf\n", word.word.c_str(), word.dist);
        resultWords.push_back(word.word);
    }
    return true;
}


