///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
//
//	Combine multiple single-view images
//	into one multi-view image.
//
//----------------------------------------------------------------------------

#include <makeMultiView.h>
#include <Image.h>
#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <ImfStandardAttributes.h>
#include <ImfMultiView.h>
#include "Iex.h"
#include <map>
#include <algorithm>
#include <iostream>


using namespace Imf;
using namespace Imath;
using namespace std;


void
makeMultiView (const vector <string> &viewNames,
	       const vector <const char *> &inFileNames,
	       const char *outFileName,
	       Compression compression,
	       bool verbose)
{
    Header header;
    Image image;
    FrameBuffer outFb;

    //
    // Write the input image files
    //

    for (int i = 0; i < viewNames.size(); ++i)
    {
	InputFile in (inFileNames[i]);

	if (verbose)
	{
	    cout << "reading file " << inFileNames[i] << " "
		    "for " << viewNames[i] << " view" << endl;
	}

	if (hasMultiView (in.header()))
	{
	    THROW (Iex::NoImplExc,
		   "The image in file " << inFileNames[i] << " is already a "
		   "multi-view image.  Cannot combine multiple multi-view "
		   "images.");
	}

	if (i == 0)
	{
	    header = in.header();
	    image.resize (header.dataWindow());
	}

	if (i > 0 && in.header().dataWindow() != header.dataWindow())
	{
	    THROW (Iex::InputExc,
		   "Image files " << inFileNames[0] << " "
		   "and " << inFileNames[i] << " have different "
		   "data windows.");
	}

	FrameBuffer inFb;

	for (ChannelList::ConstIterator j = in.header().channels().begin();
	     j != in.header().channels().end();
	     ++j)
	{
	    const Channel &inChannel = j.channel();
	    string inChanName = j.name();
	    string outChanName = insertViewName (inChanName, viewNames, i);

	    image.addChannel (outChanName, inChannel);
	    header.channels().insert (outChanName, inChannel);

	    inFb.insert  (inChanName,  image.channel(outChanName).slice());
	    outFb.insert (outChanName, image.channel(outChanName).slice());
	}

	in.setFrameBuffer (inFb);
	in.readPixels (header.dataWindow().min.y, header.dataWindow().max.y);
    }

    //
    // Write the output image file
    //

    {
	header.compression() = compression;
	addMultiView (header, viewNames);

	OutputFile out (outFileName, header);

	if (verbose)
	    cout << "writing file " << outFileName << endl;

	out.setFrameBuffer (outFb);

	out.writePixels
	    (header.dataWindow().max.y - header.dataWindow().min.y + 1);
    }
}
