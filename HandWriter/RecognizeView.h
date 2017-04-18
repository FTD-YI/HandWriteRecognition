//
//  RecognizeView.h
//  HandWriter
//
//  Created by yi on 14/12/2016.
//  Copyright © 2016 yi. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface Path : NSObject

@property(nonatomic, assign) size_t handWriteID;
@property(nonatomic, assign) int x;
@property(nonatomic, assign) int y;

- (id)initWithID:(size_t)handWriteID x:(int)x y:(int)y;

@end


@interface RecognizeView : UIView

@end
