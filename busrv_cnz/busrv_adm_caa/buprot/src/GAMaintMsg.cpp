#include "GAMaintMsg.h"

//
// Static Inits
//====================================================================


const GAMaintMsg::MAINT_PRIMITIVE GAMaintMsg::EchoTestMsg::s_primitive;
const GAMaintMsg::MAINT_PRIMITIVE GAMaintMsg::EchoTestRspMsg::s_primitive;

//
//
//===========================================================================
void GAMaintMsg::GAMaintTestMsg::set(int nofCharacters, const char* character) {

    //assert(nofCharacters <= MAX_LENGTH);
    m_nofCharacters = nofCharacters <= MAX_LENGTH ? nofCharacters : 0;
    memset(&m_character, 0, MAX_LENGTH);
	if (character)
		memcpy(m_character, character, m_nofCharacters + 1);
}
