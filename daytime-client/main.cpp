//
//  main.cpp
//  unp
//
//  Created by Ｈaochi Ｗu on 5/4/13.
//  Copyright (c) 2013 MyOrganizationName. All rights reserved.
//

#include <iostream>
#include "daytime.h"
int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Hello, World!\n";
#ifndef IPV6
    daytime(argc, argv);
#else
    daytime_v6(argc, argv);
#endif
    return 0;
}

