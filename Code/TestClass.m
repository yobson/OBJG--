#import <stdio.h>
#import <hello.h>


@implementation Hello

-(int) main:(char*) text{
	printf("Hello %s", text);
	return 0;
}

- (void) call {
	[self main:"hello"];
}



@end