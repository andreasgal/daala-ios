//
//  Shader.fsh
//  UVCPlayer
//
//  Created by Andreas Gal on 8/17/14.
//  Copyright (c) 2014 Mozilla. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
