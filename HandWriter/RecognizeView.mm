//
//  RecognizeView.m
//  HandWriter
//
//  Created by yi on 14/12/2016.
//  Copyright © 2016 yi. All rights reserved.
//

#import "RecognizeView.h"

#include <iostream>
#include "HandWriteRecognizer.hpp"

#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 50
#define RESULT_WIDTH 50

#define BACKGROUND_COLOR [UIColor whiteColor];


HandWriteRecognizer::Recognizer hwRecognizer;
HandWriteRecognizer::Character hwCharacter;

@implementation Path

- (id)initWithID:(size_t)handWriteID x:(int)x y:(int)y{
    if(self = [super init]){
        _handWriteID = handWriteID;
        _x = x;
        _y = y;
    }
    return self;
}

@end

@interface RecognizeView ()<UITableViewDataSource, UITableViewDelegate>

@property(nonatomic, assign) size_t handWriteID;
@property(nonatomic, strong) NSMutableArray *points;
@property(nonatomic, strong) UITableView *resultTableView;
@property(nonatomic, strong) NSMutableArray *results;
@property(nonatomic) dispatch_source_t timer;

@end

@implementation RecognizeView

- (id)initWithFrame:(CGRect)frame{
    if(self = [super initWithFrame:frame]){
        [self initReconizer];
        [self initViews];
    }
    return self;
}



- (void)initViews{
    self.backgroundColor = BACKGROUND_COLOR;
    UIButton *button = [[UIButton alloc] initWithFrame:CGRectMake((self.frame.size.width - BUTTON_WIDTH) / 2, (self.frame.size.height - BUTTON_HEIGHT - 50), BUTTON_WIDTH, BUTTON_HEIGHT)];
    [button setTitle:@"Reset" forState:UIControlStateNormal];
    [button setBackgroundColor:[UIColor blueColor]];
    [button addTarget:self action:@selector(reset) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:button];

    
    _resultTableView = [[UITableView alloc] initWithFrame:CGRectMake(self.frame.size.width - RESULT_WIDTH, 20, RESULT_WIDTH, self.frame.size.height - 20) style:UITableViewStylePlain];
    _resultTableView.backgroundColor = BACKGROUND_COLOR;
    _resultTableView.dataSource = self;
    _resultTableView.delegate = self;
    [_resultTableView registerClass:[UITableViewCell class] forCellReuseIdentifier:@"cellID"];
    _resultTableView.bounces = NO;
    _resultTableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self addSubview:_resultTableView];
    
}

- (void)initReconizer{
    _handWriteID = 0;
    hwCharacter.initSize(self.frame.size.width, self.frame.size.height);
    hwRecognizer.loadModelFile([[[NSBundle mainBundle] pathForResource:@"handwrite" ofType:@"model"] cStringUsingEncoding:NSUTF8StringEncoding]);
    _points = [NSMutableArray new];
    _results = [NSMutableArray new];
}

- (void)recognize{
    [_results removeAllObjects];
    std::vector<std::string> words;
    hwRecognizer.recognize(hwCharacter, words, 10);
    for(size_t i = 0; i < words.size(); i++){
        [_results addObject:[NSString stringWithCString:words[i].c_str() encoding:NSUTF8StringEncoding]];
    }
     [_resultTableView reloadData];
}

- (void)reset{
    _handWriteID = 0;
    hwCharacter.clear();
    [_points removeAllObjects];
    [_results removeAllObjects];
    [_resultTableView reloadData];
    [self setNeedsDisplay];
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if(_timer != nil){
        dispatch_source_cancel(_timer);
        _timer = nil;
    }
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:self];
    hwCharacter.addPoint((int)_handWriteID, point.x, point.y);
    [_points addObject:[[Path alloc] initWithID:_handWriteID x:(int)point.x y:(int)point.y]];
    //NSLog(@"%d::%@", _handWriteID, NSStringFromCGPoint(point));
    [self setNeedsDisplay];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:self];
    hwCharacter.addPoint((int)_handWriteID, point.x, point.y);
    [_points addObject:[[Path alloc] initWithID:_handWriteID x:(int)point.x y:(int)point.y]];
    [self setNeedsDisplay];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    _handWriteID++;
    __weak __block __typeof(self) weakSelf = self;
    _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
    dispatch_source_set_timer(_timer, dispatch_time(DISPATCH_TIME_NOW, 0.5 * NSEC_PER_SEC), DISPATCH_TIME_FOREVER, 0.1 * NSEC_PER_SEC);
    dispatch_source_set_event_handler(_timer, ^{
        [weakSelf recognize];
    });
    dispatch_resume(_timer);
}


-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetLineWidth(context, 5.0);
    CGContextSetLineCap(context, kCGLineCapRound);
    for(int i = 0; i < (int)(_points.count - 1); ++i){
        Path *lastPoint = [_points objectAtIndex:i];
        Path *nowPoint = [_points objectAtIndex:i + 1];
        if(lastPoint.handWriteID == nowPoint.handWriteID){
            CGContextMoveToPoint(context, lastPoint.x, lastPoint.y);
            CGContextAddLineToPoint(context, nowPoint.x, nowPoint.y);
            CGContextStrokePath(context);
        }
    }
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return _results.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString *cellID = @"cellID";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellID forIndexPath:indexPath];
    if(!cell){
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellID];
    }
    cell.textLabel.text = [_results objectAtIndex:indexPath.row];
    cell.textLabel.textAlignment = NSTextAlignmentLeft;
    cell.backgroundColor = BACKGROUND_COLOR;
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self reset];
}

@end
