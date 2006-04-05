/*
 * Copyright (c) 2001-2005 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 *  BLGetIOServiceForDeviceName.c
 *  bless
 *
 *  Created by Shantonu Sen on 2/7/05.
 *  Copyright 2005 Apple Computer, Inc. All rights reserved.
 *
 *  $Id: BLGetIOServiceForDeviceName.c,v 1.1.1.1 2006/01/27 22:43:54 ssen Exp $
 *
 *  $Log: BLGetIOServiceForDeviceName.c,v $
 *  Revision 1.1.1.1  2006/01/27 22:43:54  ssen
 *  bless-37.1 import
 *
 *  Revision 1.3  2005/08/22 20:49:24  ssen
 *  Change functions to take "char *foo" instead of "char foo[]".
 *  It should be semantically identical, and be more consistent with
 *  other system APIs
 *
 *  Revision 1.2  2005/06/24 16:39:51  ssen
 *  Don't use "unsigned char[]" for paths. If regular char*s are
 *  good enough for the BSD system calls, they're good enough for
 *  bless.
 *
 *  Revision 1.1  2005/02/07 21:22:38  ssen
 *  Refact lookupServiceForName and code for BLDeviceNeedsBooter
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>

#import <mach/mach_error.h>

#import <IOKit/IOKitLib.h>
#import <IOKit/IOBSD.h>

#include <CoreFoundation/CoreFoundation.h>

#include "bless.h"
#include "bless_private.h"

int BLGetIOServiceForDeviceName(BLContextPtr context, char * devName,
								io_service_t *service)
{
    io_service_t			myservice;
    io_iterator_t			services;
    kern_return_t			kret;
	mach_port_t				ourIOKitPort;

	*service = 0;
    
	// Obtain the I/O Kit communication handle.
    if((kret = IOMasterPort(bootstrap_port, &ourIOKitPort)) != KERN_SUCCESS) {
		return 1;
    }
	
	
    kret = IOServiceGetMatchingServices(ourIOKitPort,
										IOBSDNameMatching(ourIOKitPort,
														  0,
														  devName),
										&services);
    if (kret != KERN_SUCCESS) {
        return 2;
    }
    
    // Should only be one IOKit object for this volume. (And we only want one.)
    myservice = IOIteratorNext(services);
    if (!myservice) {
        IOObjectRelease(services);
        return 4;
    }
    
    IOObjectRelease(services);
    
	*service = myservice;
	
    return 0;
}