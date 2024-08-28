/*
 * GASrmMsg.h
 *
 *  Created on: May 22, 2012
 *      Author: xdtthng
 *
 *  Update: 
 *  2012-12-24  xdthng  Change api method names
 */

#ifndef GA_SRM_MSG_H_
#define GA_SRM_MSG_H_

#include "GAMsg.h"

#pragma once
#pragma pack(push, 1)

class GASrmMsg: public GAMsg
{
public:

    enum SRM_PRIMITIVE {

        // Primitives for maintenance and testing purposes
    	FIRST_RESERVED_PRIMITIVE = 10,
        SET_SBC_RELOAD_FILE = FIRST_RESERVED_PRIMITIVE,
        SET_SBC_RELOAD_FILE_RSP,
        SBC_FILE_EXIST,
        SBC_FILE_EXIST_RSP
    };

    enum ERROR_CODE {
		RC_OK = 0,					// Command was executed
		RC_NOCONTACT,				// No contact with server
		RC_FILE_NOT_FOUND,			// Requested file not found
		RC_INTERNAL_ERROR,			// Error due to FMS, Windows, ...
		RC_FILE_OUT_OF_RANGE,		// File out of Cluster Reload Range
		RC_FUNCTION_NOT_SUPPORTED,	// Interface is used when APZ Profile == 0
        LAST_ERROR_CODE = RC_FUNCTION_NOT_SUPPORTED
    };
    //
    // Text goes here
    //
    //================================================================
    class SetReloadFileMsg : public GAMsgHeader<SRM_PRIMITIVE> {

    public:
        static const SRM_PRIMITIVE s_primitive = SET_SBC_RELOAD_FILE;
    public:
        SetReloadFileMsg(u_int32 fn):
            GAMsgHeader<SRM_PRIMITIVE>(GAMsg::SRM_MESSAGE_CLASS, s_primitive, VERSION), m_fileNumber(fn) {}

        u_int32 fileNumber() const { return m_fileNumber; }
        void fileNumber(unsigned int fn) { m_fileNumber = fn; }

    private:
        u_int32 m_fileNumber;;
    };
    //
    //
    //================================================================
    class SetReloadFileRspMsg : public GARspHeader<SRM_PRIMITIVE> {
    // types
    public:
        static const SRM_PRIMITIVE s_primitive = SET_SBC_RELOAD_FILE_RSP;
    public:
        SetReloadFileRspMsg(u_int32 error)
        : GARspHeader<SRM_PRIMITIVE>(GAMsg::SRM_MESSAGE_CLASS, s_primitive, VERSION, error) { }
    private:
    };
    //
    //
    //================================================================
    class SbcFileExistMsg : public GAMsgHeader<SRM_PRIMITIVE> {
    // types
    public:
        static const SRM_PRIMITIVE s_primitive = SBC_FILE_EXIST;
    public:
        SbcFileExistMsg(unsigned int fn)
        : GAMsgHeader<SRM_PRIMITIVE>(GAMsg::SRM_MESSAGE_CLASS, s_primitive, VERSION), m_fileNumber(fn) { }
        // access
        unsigned int fileNumber() const { return m_fileNumber; }
        void fileNumber(unsigned int fn) { m_fileNumber = fn; }
        // modify
    private:
        u_int32 m_fileNumber;
    };
    //
    //
    //================================================================
    class SbcFileExistRspMsg : public GARspHeader<SRM_PRIMITIVE> {
    // types
    public:
        static const SRM_PRIMITIVE s_primitive= SBC_FILE_EXIST_RSP;
    public:
        SbcFileExistRspMsg(u_int32 error)
        : GARspHeader<SRM_PRIMITIVE>(GAMsg::SRM_MESSAGE_CLASS, s_primitive, VERSION, error) { }
        // access
        //unsigned int fileNumber() const { return m_fileNumber; }
		//unsigned int result() const { return m_result; }
        //void fileNumber(unsigned int fn) { m_fileNumber = fn; }
        // modify
        friend ostream& operator<<(ostream& , const SbcFileExistRspMsg& );
    private:
        //u_int32 m_fileNumber;
    };
    //
    //
    //================================================================
    GAMsgHeader<SRM_PRIMITIVE>* msgHeader()  {
        return reinterpret_cast<GAMsgHeader<SRM_PRIMITIVE>* >(addr());
    }
    const GAMsgHeader<SRM_PRIMITIVE>* msgHeader() const {
        return reinterpret_cast<const GAMsgHeader<SRM_PRIMITIVE>* >(addr());
    }

    GARspHeader<SRM_PRIMITIVE>* rspHeader() {
        return reinterpret_cast<GARspHeader<SRM_PRIMITIVE>* >(addr());
    }
    const GARspHeader<SRM_PRIMITIVE>* rspHeader() const {
        return reinterpret_cast<const GARspHeader<SRM_PRIMITIVE>* >(addr());
    }
};

#pragma pack(pop)

#endif /* GA_SRM_MSG_H_ */
