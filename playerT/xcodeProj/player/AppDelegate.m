//
//  AppDelegate.m
//  player
//
//  Created by liaogang on 14/10/8.
//  Copyright (c) 2014年 gang.liao. All rights reserved.
//

#import "AppDelegate.h"
#import "NSPlayListView.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    
    NSPlayListView *playlistView = [[NSPlayListView alloc] initWithNibName:@"NSPlayListView" bundle:nil];
    
    [playlistView.view setFrame: ((NSView*)self.window.contentView).bounds];
    
    playlistView.view.autoresizingMask =
    NSViewMinXMargin |
    NSViewWidthSizable |
    NSViewMaxXMargin |
    NSViewMinYMargin |
    NSViewHeightSizable	|
    NSViewMaxYMargin ;
    
    [self.window.contentView addSubview:playlistView.view];
    

    // Insert code here to initialize your application
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
