/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors: Juha Kauppinen, Mika Hokkanen
* 
* Description: Photo Browser
*
*/

#ifndef DRAWABLE_INTERFACE_H
#define DRAWABLE_INTERFACE_H

/*--------------------------------------------------------------------------*/

#include <e32std.h>
#include <e32base.h>


/*--------------------------------------------------------------------------*/

class CDrawableInterface : public CBase
	{
	protected:
		// Constructor
		CDrawableInterface();
		//virtual void ConstructL(void) =0;
	public:
		// Destructor
		virtual ~CDrawableInterface();
		
		// Pure virtual Update and Draw functions
		
		// Update, returns true if screen should be redrawn
		virtual TBool Update(void) =0;
		// Draw, takes screen size as a argument
		virtual void Draw(const TSize &aScreenSize) =0;
	};

/*--------------------------------------------------------------------------*/

#endif // DRAWABLE_INTERFACE_H
