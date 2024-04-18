/*
 * $Logfile: /DescentIII/main/BriefingParse.cpp $
 * $Revision: 11 $
 * $Date: 4/14/99 2:50a $
 * $Author: Jeff $
 *
 * Parse functions to parse a briefing file
 *
 * $Log: /DescentIII/main/BriefingParse.cpp $
 *
 * 11    4/14/99 2:50a Jeff
 * fixed some case mismatched #includes
 *
 * 10    2/17/99 6:55p Jeff
 * added jump button type.  Added no early render flag for bitmaps.  Fixed
 * color bug for type text
 *
 * 9     2/17/99 11:12a Doug
 * set movie type to TC_MOVIE_STATIC (Jeff)
 *
 * 8     1/13/99 8:01p Jeff
 * initialize text_ptr (even though it really doesn't get used till it's
 * init), to get rid of warning
 *
 * 7     1/04/99 12:32p Jeff
 * added support for mission flag parsing
 *
 * 6     11/17/98 3:39p Jeff
 * handle any length line when parsing
 *
 * 5     10/12/98 8:32p Jeff
 * changed the way focus is handled
 *
 * 4     9/17/98 2:28p Jeff
 * added focus filenames to button effect
 *
 * 3     9/11/98 6:06p Jeff
 * Briefing Editor completed
 *
 * 2     9/09/98 7:02p Jeff
 * Initial Creation
 *
 * $NoKeywords: $
 */
#include "BriefingParse.h"
#include "TelComEffects.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cfile/cfile.h"
#include "pserror.h"
#include "game.h"
#include "mem.h"
#include "voice.h"
#include "streamaudio.h"
#include "ddio.h"

//	constructor
CBriefParse::CBriefParse() {
  AddTextEffect = NULL;
  AddBmpEffect = NULL;
  AddMovieEffect = NULL;
  AddBkgEffect = NULL;
  AddPolyEffect = NULL;
  AddSoundEffect = NULL;
  AddButtonEffect = NULL;
  StartScreen = NULL;
  EndScreen = NULL;
  LoopCallback = NULL;
  SetTitle = NULL;
  SetStatic = NULL;
  SetGlitch = NULL;
  AddVoice = NULL;
  linenum = 0;
  parse_error = false;
}

//	destructor
CBriefParse::~CBriefParse() {}

//	SetCallbacks
//
//	Registers (or changes) the callbacks of the class
void CBriefParse::SetCallbacks(tBriefParseCallbacks *cb) {
  AddTextEffect = cb->AddTextEffect;
  AddBmpEffect = cb->AddBmpEffect;
  AddMovieEffect = cb->AddMovieEffect;
  AddBkgEffect = cb->AddBkgEffect;
  AddPolyEffect = cb->AddPolyEffect;
  AddSoundEffect = cb->AddSoundEffect;
  AddButtonEffect = cb->AddButtonEffect;
  StartScreen = cb->StartScreen;
  EndScreen = cb->EndScreen;
  LoopCallback = cb->LoopCallback;
  SetTitle = cb->SetTitle;
  SetStatic = cb->SetStatic;
  SetGlitch = cb->SetGlitch;
  AddVoice = cb->AddVoice;
}

////////////////////////////////////////////////////////////////////////////////

// Keyword IDs.  These must match the keyword strings below
#define K_NONE -1
#define K_SCREEN 0
#define K_BUTTON 1
#define K_TEXT 2
#define K_ENDTEXT 3
#define K_BITMAP 4
#define K_ENDSCREEN 5
#define K_MOVIE 6
#define K_POLY 7
#define K_TITLE 8
#define K_SOUND 9
#define K_STATIC 10
#define K_GLITCH 11
#define K_VOICE 12
#define NUM_KEYWORDS 13

// Button types
#define B_DISP 1    // display a screen
#define B_BACK 2    // go back a page
#define B_FORWARD 3 // go forward a page
#define B_QUIT 4    // exit TelCom

// Keywords.  These must match the keyword IDs above
char *keywords[] = {"screen", "button", "text",  "endtext", "bitmap", "endscreen", "movie",
                    "poly",   "title",  "sound", "static",  "glitch", "voice"};

#define tfxNONE 0
#define tfxFLASH 1
#define tfxSCROLL_L2R 2
#define tfxSCROLL_R2L 3
#define tfxSCROLL_T2B 4
#define tfxSCROLL_B2T 5
#define tfxFADE_IN_AND_OUT 6
#define tfxFADE_IN 7
#define tfxFADE_OUT 8

#define fntSMBRIEF 0
#define fntLGBRIEF 1
char *FontNames[] = {"sm_brief", "lg_brief", NULL};

char *TextEffectstr[] = {"None",       "flash",           "scroll_l2r", "scroll_r2l", "scroll_t2b",
                         "scroll_b2t", "fade_in_and_out", "fade_in",    "fade_out",   NULL};
#define bfxNONE 0
#define bfxFADE_IN 1
#define bfxFADE_OUT 2
#define bfxBLUR_IN 3
#define bfxBLUR_OUT 4
#define bfxSCAN_IN 5
#define bfxSCAN_OUT 6
#define bfxINVERT_IN 7
#define bfxINVERT_OUT 8
#define bfxSTRETCH_IN 9
#define bfxSTRETCH_OUT 10

char *BitmapEffectstr[] = {"None",     "Fade_in",   "Fade_out",   "Blur_in",    "Blur_out",    "Scan_in",
                           "Scan_out", "Invert_in", "Invert_out", "Stretch_in", "Stretch_out", NULL};

////////////////////////////////////////////
//	These are the types of on screen buttons
#define osbDOWN_ARROW 0
#define osbUP_ARROW 1
#define osbNEXT_PAGE 2
#define osbPREV_PAGE 3
#define osbQUIT 4
#define osbJUMP_PAGE 5

char *OnScreenButtonTypes[] = {"Down", "Up", "Next", "Prev", "Quit", "Jump", NULL};

/////////////////////////////////////////////
// These are the different ways an onscreen button will respond to mouse clicks
#define oscHOLD_DOWN 0
#define oscCLICK_DOWN 1
#define oscCLICK_UP 2

char *OnScreenButtonClickTypes[] = {"HoldDown", "ClickDown", "ClickUp", NULL};

#define PARSE_INT(i)                                                                                                   \
  do {                                                                                                                 \
    p = ParseInt(p, &(i));                                                                                             \
    if (!p)                                                                                                            \
      goto done_parsing;                                                                                               \
  } while (0)
#define PARSE_FLOAT(i)                                                                                                 \
  do {                                                                                                                 \
    p = ParseFloat(p, &(i));                                                                                           \
    if (!p)                                                                                                            \
      goto done_parsing;                                                                                               \
  } while (0)
#define PARSE_COMMA()                                                                                                  \
  do {                                                                                                                 \
    p = ParseComma(p);                                                                                                 \
    if (!p)                                                                                                            \
      goto done_parsing;                                                                                               \
  } while (0)
#define PARSE_STRING(buf)                                                                                              \
  do {                                                                                                                 \
    p = ParseString(p, buf, sizeof(buf));                                                                              \
    if (!p)                                                                                                            \
      goto done_parsing;                                                                                               \
  } while (0)
#define PARSE_TOKEN(buf)                                                                                               \
  do {                                                                                                                 \
    p = ParseToken(p, buf, sizeof(buf));                                                                               \
    if (!p)                                                                                                            \
      goto done_parsing;                                                                                               \
  } while (0)

int ReadFullLine(char **data, CFILE *ifile) {
  int counter, readin;
  char buffer[512];
  bool done;

  counter = 0;
  done = false;
  readin = 0;
  *data = NULL;
  char c;

  while (!done) {

    // read in a byte
    c = cfgetc(ifile);

    if ((c == EOF) || (!(ifile->flags & CF_TEXT) && (c == 0)) || ((ifile->flags & CF_TEXT) && (c == '\n'))) {
      // we've hit the end of the line
      done = true;
    } else {
      buffer[readin] = c;
      readin++;
    }

    // check if we have a full temp buffer
    if (readin == sizeof(buffer)) {
      // we have a full temp buffer
      char *temp_buffer;
      temp_buffer = (char *)mem_malloc(counter + sizeof(buffer)); // allocate another buffer full
      if (*data) {
        // copy over existing data
        memcpy(temp_buffer, *data, counter);
        // free it
        mem_free(*data);
      }
      // copy over new data
      memcpy(&temp_buffer[counter], buffer, sizeof(buffer));

      // reset our buffer for the temp buffer
      counter += readin;
      readin = 0;

      *data = temp_buffer;
    }
  }

  // append all the data thats in the tempbuffer into the final buffer
  char *temp_buffer;
  temp_buffer = (char *)mem_malloc(counter + readin + 1); // allocate what we need (plus for NULL term)
  if (*data) {
    // copy over existing data
    memcpy(temp_buffer, *data, counter);
    // free it
    mem_free(*data);
  }
  // copy over new data
  memcpy(&temp_buffer[counter], buffer, readin);

  counter += (readin + 1);
  *data = temp_buffer;

  temp_buffer[counter - 1] = '\0';

  return counter;
}

//	ParseBriefing
//
//	Parses the briefing file (calling the callbacks throughout), check return code
int CBriefParse::ParseBriefing(char *filename) {
  CFILE *ifile;
  bool retvalue = false;
  bool voice_def_screen = false;
  char text_buf[8192], *text_ptr = NULL;
  bool abort = 0;
  bool gottitle = false;
  char *linebuf = NULL;

  int beffect = bfxNONE;
  int x = 0, y = 0;
  char filen[40];
  bool emergency_exit = false;

  // the following block of 'globals' are for when parsing a text block, the information is stored
  // here until a $endtext is hit
  char title_buf[128];
  bool reading_text = false;
  tTextBufferDesc text_buffer_desc;
  text_buffer_desc.text_id = -1; // id # for current textblock
  text_buffer_desc.teffect = tfxNONE;

  int current_screen = -1;
  char title[100];

  sprintf(title, " ");

  mprintf((0, "Parsing <%s>\n", filename));

  // Open the file
  ifile = cfopen(filename, "rt");
  if (!ifile)
    return PBERR_FILENOTEXIST;

  // Initialize vars
  linenum = 0;
  parse_error = 0;
  title_buf[0] = 0;

  // Read & parse lines
  int bytes_read;
  char *p;

  linebuf = NULL;

  while ((!cfeof(ifile) && !parse_error && !abort)) {
    if (linebuf) {
      mem_free(linebuf);
      linebuf = NULL;
    }

    if (LoopCallback) {
      abort = (*LoopCallback)();
    }

    // Read the line
    bytes_read = ReadFullLine(&linebuf, ifile);
    linenum++;

    if (!linebuf) {
      // no more data? or empty line
      continue;
    }

    // Check for line too long
    //@@if (bytes_read >= sizeof(linebuf)-1) {
    //@@	ParseError("Line too long");
    //@@	break;
    //@@}

    // Check for keyword
    if (linebuf[0] == '$') {
      int keyword_id;

      // Get the keyword id
      p = ParseKeyword(linebuf, &keyword_id);

      switch (keyword_id) {
      case K_TITLE: {
        if (gottitle)
          ParseError("$TITLE already defined\n");
        PARSE_STRING(title);
        gottitle = true;
        if (SetTitle)
          (*SetTitle)(title);
      } break;
      case K_STATIC: {
        float s;
        PARSE_FLOAT(s);
        if (SetStatic)
          (*SetStatic)(s);
      } break;
      case K_GLITCH: {
        float g;
        PARSE_FLOAT(g);
        if (SetGlitch)
          (*SetGlitch)(g);
      } break;
      case K_SOUND: {
        // add a sound effect
        char buffer[20];
        char d[128];
        d[0] = '\0';
        TCSNDDESC snddesc;
        snddesc.caps = 0;
        bool play = false;
        snddesc.mission_mask_set = snddesc.mission_mask_unset = 0;

        while (!play) {
          PARSE_TOKEN(buffer);
          if (!stricmp(buffer, "play"))
            play = true;
          else if (!stricmp(buffer, "once")) {
            snddesc.caps |= TCSD_ONCE;
            snddesc.once = true;
          } else if (!stricmp(buffer, "starttime")) {
            snddesc.caps |= TCSD_WAITTIME;
            PARSE_FLOAT(snddesc.waittime);
          } else if (!stricmp(buffer, "isset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              snddesc.mission_mask_set |= bit;
            }
          } else if (!stricmp(buffer, "isnset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              snddesc.mission_mask_unset |= bit;
            }
          } else if (!stricmp(buffer, "desc")) {
            PARSE_STRING(d);
          } else
            ParseError("Illegal parameter in $SOUND");
        }
        PARSE_STRING(snddesc.filename);
        if (AddSoundEffect)
          (*AddSoundEffect)(&snddesc, d);
      } break;
      case K_VOICE: {
        // define the voice for this screen
        bool play = false;
        char d[128];
        d[0] = '\0';
        if (current_screen == -1)
          ParseError("Voice keyword found outside a $screen block\n");
        if (voice_def_screen)
          ParseError("A Voice is already defined for screen\n");

        char buffer[_MAX_PATH];
        char token[30];
        int vflags = 0;
        bool bitdepthset = false;
        while (!play) {
          PARSE_TOKEN(token);
          if (!stricmp(token, "play"))
            play = true;
          else if (!stricmp(token, "8bit")) {
            vflags |= VF_8BIT;
            bitdepthset = true;
          } else if (!stricmp(token, "compressed")) {
            vflags |= VF_COMPRESSED;
          } else if (!stricmp(token, "stereo")) {
            vflags |= VF_STEREO;
          } else if (!stricmp(buffer, "isset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              // snddesc.mission_mask_set |= bit;
            }
          } else if (!stricmp(buffer, "isnset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              // snddesc.mission_mask_unset |= bit;
            }
          } else if (!stricmp(token, "desc")) {
            PARSE_STRING(d);
          } else
            ParseError("Illegal parameter in $VOICE");
        }
        if (!bitdepthset)
          vflags |= VF_16BIT | VF_INTERUPT | VF_FORCE;

        PARSE_STRING(buffer);

        /*
        if(AddVoice)
                (*AddVoice)(buffer,flags);
        */

        voice_def_screen = true;
      } break;
      case K_POLY: {
        // add a poly effect
        char name[32];
        float pos_x, pos_y, pos_z;
        float rot_x, rot_y, rot_z;
        float ori_x, ori_y, ori_z;

        PARSE_FLOAT(pos_x);
        PARSE_COMMA();
        PARSE_FLOAT(pos_y);
        PARSE_COMMA();
        PARSE_FLOAT(pos_z);

        PARSE_FLOAT(ori_x);
        PARSE_COMMA();
        PARSE_FLOAT(ori_y);
        PARSE_COMMA();
        PARSE_FLOAT(ori_z);

        PARSE_FLOAT(rot_x);
        PARSE_COMMA();
        PARSE_FLOAT(rot_y);
        PARSE_COMMA();
        PARSE_FLOAT(rot_z);

        PARSE_STRING(name);
        TCPOLYDESC polydesc;
        polydesc.caps = 0;
        polydesc.pos_x = pos_x;
        polydesc.pos_y = pos_y;
        polydesc.pos_z = pos_z;
        polydesc.ori_x = ori_x;
        polydesc.ori_y = ori_y;
        polydesc.ori_z = ori_z;
        polydesc.rot_x = rot_x;
        polydesc.rot_y = rot_y;
        polydesc.rot_z = rot_z;
        polydesc.mission_mask_set = 0;
        polydesc.mission_mask_unset = 0;
        strcpy(polydesc.polyname, name);
        if (AddPolyEffect)
          (*AddPolyEffect)(&polydesc, NULL);
      } break;
      case K_BUTTON: {
        if (!ParseButtonEffect(p))
          goto done_parsing;
      } break;
      case K_SCREEN: {
        // define a screen
        char description[128];
        char buffer[64];
        bool done = false;
        bool layout_found = false;
        char layout_scr[_MAX_PATH];
        uint mission_mask_set = 0, mission_mask_unset = 0;
        description[0] = '\0';
        PARSE_INT(current_screen);

        if ((current_screen < 0) || (current_screen >= MAX_TELCOM_SCREENS))
          ParseError("Illegal Screen number\n");

        while (!done) {
          PARSE_TOKEN(buffer);
          if (!stricmp(buffer, "start")) {
            done = 1;
          } else if (!stricmp(buffer, "desc")) {
            PARSE_STRING(description);
          } else if (!stricmp(buffer, "isset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              mission_mask_set |= bit;
            }
          } else if (!stricmp(buffer, "isnset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              mission_mask_unset |= bit;
            }
          } else if (!stricmp(buffer, "layout")) {
            PARSE_STRING(layout_scr);
            layout_found = true;
          }
        }

        if (StartScreen)
          (*StartScreen)(current_screen, description, (layout_found) ? layout_scr : NULL, mission_mask_set,
                         mission_mask_unset);

        voice_def_screen = false;
      } break;
      case K_ENDSCREEN: {
        if ((current_screen >= 0) && (current_screen < MAX_TELCOM_SCREENS)) {
          if (EndScreen)
            (*EndScreen)();
        } else {
          // invalid screen
          ParseError("Illegal screen number or missing $SCREEN\n");
        }
        current_screen = -1;
      } break;
      case K_TEXT: {
        reading_text = true;
        text_ptr = text_buf;
        text_buffer_desc.text_id = -1;

        if (!ParseTextEffect(p, &text_buffer_desc))
          goto done_parsing;
      } break;
      case K_ENDTEXT: {
        if (!reading_text)
          ParseError("Found $ENDTEXT without $TEXT");
        reading_text = false;
        text_buffer_desc.textdesc.looping = false;
        text_buffer_desc.textdesc.caps |= TCTD_LOOPING;
        if (AddTextEffect)
          (*AddTextEffect)(&text_buffer_desc.textdesc, text_buf, text_buffer_desc.description,
                           text_buffer_desc.text_id);
        text_buffer_desc.text_id = -1;
        text_ptr = NULL;
      } break;
      case K_BITMAP: {
        char buffer[64];
        char d[128];
        d[0] = '\0';
        bool done = 0;
        bool found;
        float speed, starttime;
        int i;

        speed = 1.0f;
        starttime = 0;

        TCBMPDESC bmpdesc;
        bmpdesc.caps = 0;
        bmpdesc.type = TC_BMP_STATIC;
        bmpdesc.flags = 0;
        bmpdesc.mission_mask_set = 0;
        bmpdesc.mission_mask_unset = 0;
        while (!done) {
          PARSE_TOKEN(buffer);
          if (!stricmp(buffer, "show")) {
            done = 1;
          } else if (!stricmp(buffer, "no_early_rend")) {
            bmpdesc.flags |= TC_NOEARLYRENDER;
          } else if (!stricmp(buffer, "effect")) {
            found = false;
            i = 0;
            PARSE_TOKEN(buffer);
            while (!found) {
              if (!BitmapEffectstr[i])
                found = true;
              else if (!strnicmp(BitmapEffectstr[i], buffer, strlen(BitmapEffectstr[i]))) {
                beffect = i;
                found = true;
              }
              i++;
            }
            bmpdesc.type = TC_BMP_STATIC;
            switch (beffect) {
            case bfxBLUR_IN:
              bmpdesc.type = TC_BMP_BLUR;
              bmpdesc.flags |= TC_BMPF_IN;
              break;
            case bfxBLUR_OUT:
              bmpdesc.type = TC_BMP_BLUR;
              bmpdesc.flags |= TC_BMPF_OUT;
              break;
            case bfxSCAN_IN:
              bmpdesc.type = TC_BMP_SCANLINE;
              bmpdesc.flags |= TC_BMPF_IN;
              break;
            case bfxSCAN_OUT:
              bmpdesc.type = TC_BMP_SCANLINE;
              bmpdesc.flags |= TC_BMPF_OUT;
              break;
            case bfxINVERT_IN:
              bmpdesc.type = TC_BMP_INVERT;
              bmpdesc.flags |= TC_BMPF_IN;
              break;
            case bfxINVERT_OUT:
              bmpdesc.type = TC_BMP_INVERT;
              bmpdesc.flags |= TC_BMPF_OUT;
              break;
            case bfxSTRETCH_IN:
              bmpdesc.type = TC_BMP_STRETCH;
              bmpdesc.flags |= TC_BMPF_IN;
              break;
            case bfxSTRETCH_OUT:
              bmpdesc.type = TC_BMP_STRETCH;
              bmpdesc.flags |= TC_BMPF_OUT;
              break;
            case bfxFADE_IN:
            case bfxFADE_OUT:
            default:
              bmpdesc.type |= TC_BMP_STATIC;
              break;
            }
          } else if (!stricmp(buffer, "speed")) {
            PARSE_FLOAT(speed);
            bmpdesc.speed = speed;
            bmpdesc.caps |= TCBD_SPEED;
          } else if (!stricmp(buffer, "starttime")) {
            PARSE_FLOAT(starttime);
            bmpdesc.waittime = starttime;
            bmpdesc.caps |= TCBD_WAITTIME;
          } else if (!stricmp(buffer, "position")) {
            PARSE_INT(x);
            PARSE_COMMA();
            PARSE_INT(y);
            bmpdesc.caps |= TCBD_XY;
            bmpdesc.x = x;
            bmpdesc.y = y;
          } else if (!stricmp(buffer, "isset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              bmpdesc.mission_mask_set |= bit;
            }
          } else if (!stricmp(buffer, "isnset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              bmpdesc.mission_mask_unset |= bit;
            }
          } else if (!stricmp(buffer, "desc")) {
            PARSE_STRING(d);
          } else
            ParseError("Illegal parameter in $BITMAP");
        }
        PARSE_STRING(filen);
        strcpy(bmpdesc.filename, filen);
        if (AddBmpEffect)
          (*AddBmpEffect)(&bmpdesc, d);
      } break;
      case K_MOVIE: {
        int x = 0, y = 0;
        char movie_name[128];
        char d[128];
        d[0] = '\0';
        float fps = 20.0;
        TCMOVIEDESC moviedesc;
        float starttime = 0;
        moviedesc.caps = 0;
        bool done = false;
        char buffer[30];
        moviedesc.caps = 0;
        moviedesc.mission_mask_set = 0;
        moviedesc.mission_mask_unset = 0;
        moviedesc.type = TC_MOVIE_STATIC;
        while (!done) {
          PARSE_TOKEN(buffer);

          if (!stricmp(buffer, "show")) {
            done = 1;
          } else if (!stricmp(buffer, "position")) {
            PARSE_INT(x);
            PARSE_COMMA();
            PARSE_INT(y);
            moviedesc.x = x;
            moviedesc.y = y;
            moviedesc.caps |= TCMD_XY;
          } else if (!stricmp(buffer, "fps")) {
            PARSE_FLOAT(fps);
            moviedesc.fps = fps;
            moviedesc.caps |= TCMD_FPS;
          } else if (!stricmp(buffer, "looping")) {
            moviedesc.looping = true;
            moviedesc.caps |= TCMD_LOOPING;
          } else if (!stricmp(buffer, "starttime")) {
            PARSE_FLOAT(starttime);
            moviedesc.waittime = starttime;
            moviedesc.caps |= TCMD_WAITTIME;
          } else if (!stricmp(buffer, "isset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              moviedesc.mission_mask_set |= bit;
            }
          } else if (!stricmp(buffer, "isnset")) {
            int value, bit = 0x01;
            PARSE_INT(value);
            if (value >= 0 && value < 32) {
              bit = bit << value;
              moviedesc.mission_mask_unset |= bit;
            }
          } else if (!stricmp(buffer, "desc")) {
            PARSE_STRING(d);
          } else
            ParseError("Illegal parameter in $MOVIE");
        }

        PARSE_STRING(movie_name);
        strcpy(moviedesc.filename, movie_name);
        if (AddMovieEffect)
          (*AddMovieEffect)(&moviedesc, d);
      } break;
      case K_NONE:
        ParseError("Unknown keyword", p);
        break;
      default:
        Int3(); // Programming error: unknown keyword ID
      }
      continue;
    } else if (reading_text && text_ptr) { // a line of text
      if (text_ptr != text_buf)
        *text_ptr++ = '\n';
      if (text_ptr - text_buf + strlen(linebuf) > sizeof(text_buf))
        ParseError("Text buffer overflow");
      else {
        strcpy(text_ptr, linebuf);
        text_ptr += strlen(text_ptr);
      }
    } else if ((linebuf[0] == ';') || (linebuf[0] == 0)) // Check for comment or blank line
      continue;
    else {
      // Not a comment, keyword, or text, so it's an error
      ParseError("Unexpected text on line", p);
    }
  }

done_parsing:;

  if (abort)
    mprintf((0, "Parse aborted\n"));
  else if (!parse_error)
    if (reading_text)
      ParseError("Missing '$endtext'");
    else
      mprintf((0, "Parse sucessful\n"));

  // Close the file
  cfclose(ifile);

  if (linebuf) {
    // make sure all memory is freed
    mem_free(linebuf);
    linebuf = NULL;
  }

  return PBERR_NOERR;
}

// Parses the keyword description for an OnScreenButton
bool CBriefParse::ParseButtonEffect(char *p) {
  int type = osbPREV_PAGE, ctype = oscCLICK_UP;
  // creates an input button
  int x, y, screen;
  int osflags = 0;
  char buffer[20];
  bool show = false, flasher = false;
  int p_id = -1, s_id = -1;
  int id = -1;
  float flash_time = -1;
  char flash_name[256];
  char flash_name_focus[256];
  char d[128];
  uint mission_mask_set = 0, mission_mask_unset = 0;
  d[0] = '\0';

  strcpy(flash_name, " ");
  strcpy(flash_name_focus, " ");
  screen = -1;

  PARSE_INT(x);
  PARSE_COMMA();
  PARSE_INT(y);
  PARSE_TOKEN(buffer);

  while (!show) {
    if (!stricmp(buffer, "show"))
      show = true;
    else if (!stricmp(buffer, "flash")) {
      flasher = true;
      osflags |= OBF_FLASH;
      PARSE_FLOAT(flash_time);
      PARSE_STRING(flash_name);
      PARSE_STRING(flash_name_focus);
    } else if (!stricmp(buffer, "id")) {
      PARSE_INT(id);
    } else if (!stricmp(buffer, "parent_id")) {
      PARSE_INT(p_id);
    } else if (!stricmp(buffer, "sibling_id")) {
      PARSE_INT(s_id);
    } else if (!stricmp(buffer, "desc")) {
      PARSE_STRING(d);
    } else if (!stricmp(buffer, "type")) {
      char btype_buf[30];
      bool found = false;
      int i = 0;

      PARSE_TOKEN(btype_buf);
      while (!found) {
        if (!OnScreenButtonTypes[i])
          found = true;
        else if (!strnicmp(OnScreenButtonTypes[i], btype_buf, strlen(OnScreenButtonTypes[i]))) {
          type = i;
          found = true;
        }
        i++;
      }
      if (type == osbJUMP_PAGE) {
        // parse what screen to jump to
        PARSE_INT(screen);
      }
    } else if (!stricmp(buffer, "click")) {
      char ctype_buf[30];
      bool found = false;
      int i = 0;

      PARSE_TOKEN(ctype_buf);
      while (!found) {
        if (!OnScreenButtonClickTypes[i])
          found = true;
        else if (!strnicmp(OnScreenButtonClickTypes[i], ctype_buf, strlen(OnScreenButtonClickTypes[i]))) {
          ctype = i;
          found = true;
        }
        i++;
      }
    } else if (!stricmp(buffer, "isset")) {
      int value, bit = 0x01;
      PARSE_INT(value);
      if (value >= 0 && value < 32) {
        bit = bit << value;
        mission_mask_set |= bit;
      }
    } else if (!stricmp(buffer, "isnset")) {
      int value, bit = 0x01;
      PARSE_INT(value);
      if (value >= 0 && value < 32) {
        bit = bit << value;
        mission_mask_unset |= bit;
      }
    } else if (!stricmp(buffer, "glow")) {
      flasher = true;
      osflags |= OBF_GLOW;
    } else
      ParseError("Illegal parameter in $BUTTON");

    if (!show)
      PARSE_TOKEN(buffer);
  }

  char name[256];
  char name_focus[256];
  PARSE_STRING(name);
  PARSE_STRING(name_focus);

  if (flasher && osflags == OBF_GLOW) {
    // see if user specified a glow, but gave no flash, so we need to get a filename
    PARSE_STRING(flash_name);
    PARSE_STRING(flash_name_focus);
    flash_time = -1;
  }

  // Fill in the button description struct
  TCBUTTONDESC desc;
  desc.sibling_id = s_id;
  desc.parent_id = p_id;
  desc.mission_mask_set = mission_mask_set;
  desc.mission_mask_unset = mission_mask_unset;

  // determine what type of button this is
  switch (type) {
  case osbDOWN_ARROW:
    desc.button_type = BUTT_DOWNARROW;
    break;
  case osbUP_ARROW:
    desc.button_type = BUTT_UPARROW;
    break;
  case osbNEXT_PAGE:
    desc.button_type = BUTT_NEXTPAGE;
    break;
  case osbPREV_PAGE:
    desc.button_type = BUTT_PREVPAGE;
    break;
  case osbQUIT:
    desc.button_type = BUTT_QUIT;
    break;
  case osbJUMP_PAGE:
    desc.button_type = BUTT_JUMP;
    desc.jump_page = screen;
    break;
  default:
    Int3(); // Illegal button type
    desc.button_type = BUTT_NEXTPAGE;
    break;
  };

  // determine what type of click the button wants
  switch (ctype) {
  case oscHOLD_DOWN:
    desc.click_type = CLICKTYPE_DOWN;
    break;
  case oscCLICK_DOWN:
    desc.click_type = CLICKTYPE_CLICKDOWN;
    break;
  case oscCLICK_UP:
    desc.click_type = CLICKTYPE_CLICKUP;
    break;
  default:
    Int3(); // Illegal click type
    desc.click_type = CLICKTYPE_CLICKUP;
    break;
  };

  // fill in the rest of the information and create the button
  desc.x = x;
  desc.y = y;
  strcpy(desc.filename, name);
  strcpy(desc.filename_focus, name_focus);
  strcpy(desc.flash_filename, flash_name);
  strcpy(desc.flash_filename_focus, flash_name_focus);
  desc.flash_time = flash_time;
  desc.osflags = osflags;
  desc.flasher = flasher;

  if (AddButtonEffect)
    (*AddButtonEffect)(&desc, d, id);
  return true;

done_parsing:
  return false;
}

//	Parses a text keyword description
bool CBriefParse::ParseTextEffect(char *p, tTextBufferDesc *tbd) {
  tbd->textdesc.caps = 0;
  tbd->textdesc.type = TC_TEXT_STATIC;
  tbd->textdesc.flags = 0;
  tbd->textdesc.mission_mask_set = 0;
  tbd->textdesc.mission_mask_unset = 0;
  char buffer[64];
  bool done = 0;
  bool found;
  float speed, starttime;
  int i, lx, rx, ty, by;
  tbd->description[0] = '\0';

  while (!done) {
    PARSE_TOKEN(buffer);

    if (!stricmp(buffer, "show")) {
      done = 1;
    } else if (!stricmp(buffer, "id")) {
      PARSE_INT(tbd->text_id);
    } else if (!stricmp(buffer, "desc")) {
      PARSE_STRING(tbd->description);
    } else if (!stricmp(buffer, "isset")) {
      int value, bit = 0x01;
      PARSE_INT(value);
      if (value >= 0 && value < 32) {
        bit = bit << value;
        tbd->textdesc.mission_mask_set |= bit;
      }
    } else if (!stricmp(buffer, "isnset")) {
      int value, bit = 0x01;
      PARSE_INT(value);
      if (value >= 0 && value < 32) {
        bit = bit << value;
        tbd->textdesc.mission_mask_unset |= bit;
      }
    } else if (!stricmp(buffer, "tabstop")) {
      tbd->textdesc.caps |= TCTD_TABSTOP;
    } else if (!stricmp(buffer, "effect")) {
      found = false;
      i = 0;
      PARSE_TOKEN(buffer);
      while (!found) {
        if (!TextEffectstr[i])
          found = true;
        else if (!strnicmp(TextEffectstr[i], buffer, strlen(TextEffectstr[i]))) {
          tbd->teffect = i;
          found = true;
        }
        i++;
      }
      switch (tbd->teffect) {
      case tfxFLASH:
        tbd->textdesc.type = TC_TEXT_FLASH;
        break;
      case tfxSCROLL_L2R:
        tbd->textdesc.type = TC_TEXT_SCROLL;
        tbd->textdesc.flags = TC_TEXTF_L2R;
        break;
      case tfxSCROLL_R2L:
        tbd->textdesc.type = TC_TEXT_SCROLL;
        tbd->textdesc.flags = TC_TEXTF_R2L;
        break;
      case tfxSCROLL_T2B:
        tbd->textdesc.type = TC_TEXT_SCROLL;
        tbd->textdesc.flags = TC_TEXTF_T2B;
        break;
      case tfxSCROLL_B2T:
        tbd->textdesc.type = TC_TEXT_SCROLL;
        tbd->textdesc.flags = TC_TEXTF_B2T;
        break;
      case tfxFADE_IN:
        tbd->textdesc.type = TC_TEXT_FADE;
        tbd->textdesc.flags = TC_TEXTF_IN;
        break;
      case tfxFADE_OUT:
        tbd->textdesc.type = TC_TEXT_FADE;
        tbd->textdesc.flags = TC_TEXTF_OUT;
        break;
      case tfxFADE_IN_AND_OUT:
        tbd->textdesc.type = TC_TEXT_FADE;
        tbd->textdesc.flags = TC_TEXTF_PINGPONG;
        break;
      default:
        break;
      }
    } else if (!stricmp(buffer, "scroll")) {
      tbd->textdesc.caps |= TCTD_SCROLL;
    } else if (!stricmp(buffer, "speed")) {
      PARSE_FLOAT(speed);
      tbd->textdesc.speed = speed;
      tbd->textdesc.caps |= TCTD_SPEED;
    } else if (!stricmp(buffer, "color")) {
      int r, g, b;
      PARSE_INT(r);
      PARSE_COMMA();
      PARSE_INT(g);
      PARSE_COMMA();
      PARSE_INT(b);
      tbd->textdesc.color = GR_RGB(r, g, b);
      tbd->textdesc.caps |= TCTD_COLOR;
    } else if (!stricmp(buffer, "font")) {
      char buffer[20];
      bool found = false;
      int i = 0;
      int feffect = 0;
      int text_font;

      PARSE_TOKEN(buffer);
      while (!found) {
        if (!FontNames[i])
          found = true;
        else if (!strnicmp(FontNames[i], buffer, strlen(FontNames[i]))) {
          feffect = i;
          found = true;
        }
        i++;
      }
      switch (feffect) {
      case fntSMBRIEF:
        text_font = BRIEF_FONT_INDEX;
        break;
      case fntLGBRIEF:
        text_font = BBRIEF_FONT_INDEX;
        break;
      }

      tbd->textdesc.font = text_font;
      tbd->textdesc.caps |= TCTD_FONT;
    } else if (!stricmp(buffer, "starttime")) {
      PARSE_FLOAT(starttime);
      tbd->textdesc.waittime = starttime;
      tbd->textdesc.caps |= TCTD_WAITTIME;
    } else if (!stricmp(buffer, "box")) {
      PARSE_INT(lx);
      PARSE_COMMA();
      PARSE_INT(rx);
      PARSE_COMMA();
      PARSE_INT(ty);
      PARSE_COMMA();
      PARSE_INT(by);
      tbd->textdesc.textbox.left = lx;
      tbd->textdesc.textbox.right = rx;
      tbd->textdesc.textbox.top = ty;
      tbd->textdesc.textbox.bottom = by;
      tbd->textdesc.caps |= TCTD_TEXTBOX;
    } else
      ParseError("Illegal parameter in $TEXT");
  }

  return true;

done_parsing:
  return false;
}

// Generates an parsing error
void CBriefParse::ParseError(char *msg, char *p) {
  mprintf((0, "ERROR, line %d: %s\n", linenum, msg));
  if (p) {
    mprintf((0, "  %s\n", p));
  }

  parse_error = 1;
}

// Parses a keyword
char *CBriefParse::ParseKeyword(char *p, int *keyword_id) {
  int i;

  *keyword_id = K_NONE;

  ASSERT(*p == '$');

  for (i = 0, p++; i < NUM_KEYWORDS; i++) {
    int len = strlen(keywords[i]);

    if (!strnicmp(p, keywords[i], len)) {
      if (!isalnum(p[len])) {
        *keyword_id = i;
        return p + strlen(keywords[i]);
      }
    }
  }
  // Error -- return initial pointer
  return p;
}

// Removes whitespace from the start of the given string.
// Returns a pointer to the first non-white character
char *CBriefParse::SkipWhite(char *p) {
  while (isspace(*p))
    p++;

  return p;
}

// Parses a sequence of non-space characters
char *CBriefParse::ParseToken(char *p, char *buf, int bufsize) {
  char *save_p;

  p = SkipWhite(p);

  save_p = p;

  while (!isspace(*p) && (*p != ',') && *p && --bufsize)
    *buf++ = *p++;

  *buf = 0;

  // Check for buffer overflow
  if (bufsize <= 0) {
    ParseError("Token too long", save_p);
    return NULL;
  }

  return p;
}

// Parses a quoted string
// Returns true if got string ok, else false
char *CBriefParse::ParseString(char *p, char *buf, int bufsize) {
  char *save_p;

  p = SkipWhite(p);

  save_p = p;

  if (*p != '"') {
    ParseError("Missing quote", p);
    return NULL;
  } else
    p++; // skip initial quote

  // Copy chars until endquote or out of space
  while (*p && (*p != '"') && --bufsize)
    *buf++ = *p++;

  // Check for buffer overflow
  if (bufsize <= 0) {
    ParseError("String too long", save_p);
    return NULL;
  }

  // Check for missing endquote
  if (!*p) {
    ParseError("Missing endquote", save_p);
    return NULL;
  }

  // Write terminator
  *buf = 0;

  // Return new pointer (move over a char for end quote)
  return p + 1;
}

// Parses an integer value
char *CBriefParse::ParseInt(char *p, int *i) {
  char *t;
  bool got_digit = 0;

  t = p = SkipWhite(p);

  // Check for negative sign
  if (*t == '-')
    t++;

  // Check for valid characters in number
  while (isdigit(*t)) {
    t++;
    got_digit = 1;
  }

  // Make sure at least one digit
  if (!got_digit)
    ParseError("Expecting digit", p);

  // Check for valid seperator, and convert
  if ((*t == 0) || isspace(*t) || (*t == ',')) { // valid number
    *i = atoi(p);
    return t;
  } else {
    ParseError("Invalid character in integer", p);
    return NULL;
  }
}

// Parses a floating-point value
char *CBriefParse::ParseFloat(char *p, float *f) {
  char *t;
  bool got_digit = 0;

  t = p = SkipWhite(p);

  // Check for negative sign
  if (*t == '-')
    t++;

  // Check for valid characters in number
  while (isdigit(*t)) {
    got_digit = 1;
    t++;
  }

  // Check for decimal point
  if (*t == '.') {
    t++;

    // Check for more digits
    while (isdigit(*t)) {
      got_digit = 1;
      t++;
    }
  }

  // Make sure at least one digit
  if (!got_digit) {
    ParseError("Expecting digit", p);
    return NULL;
  }

  // Check for valid seperator, and convert
  if ((*t == 0) || isspace(*t) || (*t == ',')) { // valid number
    *f = atof(p);
    return t;
  } else {
    ParseError("Invalid character in floating-point number", p);
    return NULL;
  }
}

// Parses a comma
char *CBriefParse::ParseComma(char *p) {
  p = SkipWhite(p);

  if (*p != ',') {
    ParseError("Comma expected");
    return NULL;
  }

  return p + 1;
}
