//
//  AppDelegate.m
//  HandWriter
//
//  Created by yi on 13/12/2016.
//  Copyright © 2016 yi. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"


@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.window.rootViewController = [ViewController new];
    [self.window makeKeyAndVisible];
    return YES;
}



@end
