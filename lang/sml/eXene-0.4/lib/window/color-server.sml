(* color-server.sml
 *
 * COPYRIGHT (c) 1992 by AT&T Bell Laboratories.  See COPYRIGHT file for details.
 *
 * The color server manages colors for a given screen.
 *)

structure ColorServer : COLOR_SERVER =
  struct

(** ??? where is this raised ??? **)
    exception NoColorCell

    structure X : sig
	datatype rgb = RGB of {red : int, green : int, blue : int}
      end = XProtTypes
    open X

  (* color specifications.  Eventually, this will be extended to R5
   * device independent color specifications.
   *)
    datatype color_spec
      = CMS_Name of string
      | CMS_RGB of {red : int, green : int, blue : int}

    val whiteRGB = RGB{red=65535, green=65535, blue=65535}
    val blackRGB = RGB{red=0, green=0, blue=0}

    datatype color = COLOR of {		(* A color in the colormap *)
	pixel : XProtTypes.pixel,	  (* Its pixel value *)
	rgb : rgb			  (* Its RGB value *)
      }

  (* The following are used for drawing 0s or 1s on pixmaps.
   * Note that the rgb fields are bogus.
   *)
    val color0 = COLOR{pixel = XProtTypes.PIXEL 0, rgb = RGB{red=0, green=0, blue=0}}
    val color1  = COLOR{pixel = XProtTypes.PIXEL 1, rgb = RGB{red=0, green=0, blue=0}}

    datatype reply_msg
      = ReplyError
      | ReplyColor of color

    datatype color_server = CS of {
	req_ch : color_spec CML.chan,
	reply_ch : reply_msg CML.chan,
	black : color,
	white : color
      }

  (* tables mapping RGB values to colors *)
    structure RGBTbl = HashTable (struct
	  type hash_key = rgb
	  fun sameKey (k1 : hash_key, k2) = (k1 = k2)
	  fun hashVal (RGB{red, green, blue}) = red+green+blue
	end);
    type rgb_tbl = color RGBTbl.hash_table

  (* tables mapping color names to colors *)
    type cname_key = (string * int)
    fun mkCNameKey s = let val s' = CType.toLower s
	  in
	    (s', HashString.hashString s')
	  end
    structure CNameTbl = HashTable (struct
	  type hash_key = cname_key
	  fun sameKey ((s1, h1) : hash_key, (s2, h2)) = ((h1 = h2) andalso (s1 = s2))
	  fun hashVal (_, h) = h
	end);
    type cname_tbl = color CNameTbl.hash_table

  (* Create a color server for a given screen.  Currently, this manages access
   * to the default colormap of the screen.
   * NOTE: once colors are finalized objects, we will need to have a pixel-to-color
   * map, to guarantee that each pixel has only one color object.
   *)
    fun mkColorServer (
	  XDisplay.XDPY{conn, ...}, XDisplay.XSCR{cmap, black, white, ...}
	) = let
	  open CML
	  exception NotFound
	  val rgbTbl : rgb_tbl = RGBTbl.mkTable(32, NotFound)
	  val rgbIns = RGBTbl.insert rgbTbl
	  val rgbFind = RGBTbl.peek rgbTbl
	  val cnameTbl : cname_tbl = CNameTbl.mkTable(32, NotFound)
	  val cnameIns = CNameTbl.insert cnameTbl
	  val cnameFind = CNameTbl.peek cnameTbl
	  val requestReply = XIo.requestReply conn
	  fun allocNamedColor cname = let
		val msg = XRequest.encodeAllocNamedColor{cmap = cmap, name = cname}
		in
		  XReply.decodeAllocNamedColorReply (sync (requestReply msg))
		end
	  fun allocRGBColor rgb = let
		val msg = XRequest.encodeAllocColor{cmap = cmap, color = rgb}
		in
		  XReply.decodeAllocColorReply (sync (requestReply msg))
		end
	  fun findColorByName cname = let
		val key = mkCNameKey cname
		in
		  case (cnameFind key)
		   of NONE => let
			val {pixel, visual_rgb, ...} = allocNamedColor cname
			val c = COLOR{pixel = pixel, rgb = visual_rgb}
			in
			  cnameIns (key, c);
			  rgbIns (visual_rgb, c);
			  c
			end
		    | (SOME c) => c
		  (* end case *)
		end (* findColorByName *)
	  fun findColorByRGB rgb = (case (rgbFind rgb)
		 of NONE => let
		      val {pixel, visual_rgb} = allocRGBColor rgb
		      val c = COLOR{pixel=pixel, rgb=visual_rgb}
		      in
			rgbIns (rgb, c);
			if (rgb <> visual_rgb) then rgbIns (visual_rgb, c) else ();
			c
		      end
		  | (SOME c) => c
		(* end case *))
	  fun handleReq (CMS_Name cname) = findColorByName cname
	    | handleReq (CMS_RGB rgb) = findColorByRGB (RGB rgb)
	  val reqCh = channel() and replyCh = channel()
	  fun serverLoop () = let
		val res = (ReplyColor(handleReq (accept reqCh))) handle _ => ReplyError
		in
		  send(replyCh, res);
		  serverLoop()
		end (* serverLoop *)
	  val black = COLOR{pixel=black, rgb=blackRGB}
	  val white = COLOR{pixel=white, rgb=whiteRGB}
	  in
	    rgbIns (blackRGB, black);
	    rgbIns (whiteRGB, white);
	    XDebug.xspawn("ColorServer", serverLoop);
	    CS{req_ch = reqCh, reply_ch = replyCh, black = black, white = white}
	  end (* mkColorServer *)

    exception BadRGB
    fun validRGB ({red, green, blue}) =
	  (XValid.valid16 red)
	  andalso (XValid.valid16 green)
	  andalso (XValid.valid16 blue)
    fun chkRGBVal c = if (validRGB c) then () else raise BadRGB

    fun chkRGB (rgb as (RGB v)) = (chkRGBVal v; rgb)

    local
      fun get (CS{req_ch, reply_ch, ...}, colorSpec) = (
	    CML.send (req_ch, colorSpec);
	    case (CML.accept reply_ch)
	     of ReplyError => raise NoColorCell
	      | (ReplyColor c) => c
	    (* end case *))
    in
    fun getColor cs (spec as (CMS_RGB rgb)) = (chkRGBVal rgb; get(cs, spec))
      | getColor cs spec = get(cs, spec)
    end

    fun blackOf (CS{black, ...}) = black
    fun whiteOf (CS{white, ...}) = white

    fun sameColor(COLOR{pixel=p1, rgb=rgb1}, COLOR{pixel=p2, rgb=rgb2}) =
	  ((p1 = p2) orelse (rgb1 = rgb2))

  end (* ColorServer *)
