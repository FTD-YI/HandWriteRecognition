//
//  ViewController.m
//  HandWriter
//
//  Created by yi on 13/12/2016.
//  Copyright © 2016 yi. All rights reserved.
//

#import "ViewController.h"
#import "RecognizeView.h"


@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view addSubview:[[RecognizeView alloc] initWithFrame:self.view.bounds]];
}






@end
