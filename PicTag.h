#ifndef _PicTag_PicTag_h
#define _PicTag_PicTag_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <PicTag/PicTag.lay>
#include <CtrlCore/lay.h>

class PicTag : public WithPicTagLayout<TopWindow> {
public:
	PicTag();
};

#endif
