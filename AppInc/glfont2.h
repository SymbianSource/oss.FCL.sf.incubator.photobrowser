//*******************************************************************
// glfont2.h -- Header for glfont2.cpp
// Copyright (c) 1998-2002 Brad Fish
// See glfont.html for terms of use
// May 14, 2002
//
// Symbian OS port - June 2007
// Luis Valente - lpvalente@gmail.com
//
//*******************************************************************
 
#ifndef GLFONT2_H
#define GLFONT2_H
 
#include <e32base.h>
#include <GLES/gl.h>
 
//_____________________________________________________________________________
//
// Simple class to output text as texture-mapped triangles. Does not support
// unicode strings. Reference point when drawing: top-left.
//
 
class GLFont
{	
  public:
 
   /**
    * Factory-method.
    */
   static GLFont* NewL (const TDesC & aFilename);
 
  public:
 
   /**
    * Destructor.
    */		
   ~GLFont ();
 
  public:
 
 
   /**
    * Retrieves the texture width and height.
    */	
   void GetTexSize (TInt & aWidth, TInt & aHeight);
 
   /**
    * Retrieves the character interval.
    */	
   void GetCharInterval (TInt & aStart, TInt & aEnd);
 
   /**
    * Retrieves the character dimensions.
    */
   void GetCharSize (TText8 c, TInt & aWidth, TInt aHeight);
 
 
   /**
    * Calculates the dimensions of a string.
    */
   void GetStringSize (const TDesC8 & aText, TInt & aWidth, TInt & aHeight);
 
   /**
    * Renders a string.
    */
   void DrawString (const TDesC8 & aText, GLfixed aX, GLfixed aY);	
 
   /**
    * Sets required states for the font.
    */
   void BeginDraw ()
   {			
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_TEXTURE_2D);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
   }
 
   /**
    * Turns off required states.
    */
   void EndDraw ()
   {			
    glDisable (GL_BLEND);			
    glDisable (GL_TEXTURE_2D);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
   }		
 
 private:
 
    /**
     * Default constructor.
     */
   GLFont ();
 
   /**
    * Final part of the two-phase constructor.
    */
   void ConstructL (const TDesC & aFilename);
 
   /**
    * Loads the font file.
    */
   void LoadFileL (RFs & aFs, const TDesC & aFilename);		
 
   /**
    * Destroys the font.
    */		
   void Destroy ();		
 
 
  private:	
 
   // single character
   struct GLFontChar
   {
      GLfixed dx, dy;
      GLfixed tx1, ty1;
      GLfixed tx2, ty2;
   };
 
   // font header
   struct GLFontHeader
   {
      GLuint tex;
      TInt   texWidth, texHeight;
      TInt   startChar, endChar;
      GLFontChar *chars;
   };			
 
 private:
 
   GLFontHeader iHeader;
};
 
//*******************************************************************
#endif