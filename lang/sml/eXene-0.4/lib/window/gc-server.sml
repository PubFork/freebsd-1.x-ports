(* gc-server.sml
 *
 * COPYRIGHT (c) 1990,1991 by John H. Reppy.  See COPYRIGHT file for details.
 *
 * This is the graphics context server.  It is responsible for mapping
 * the immutable pens provided by this layer onto the mutable graphics
 * contexts provided by the X-server.
 *
 * TODO:
 *  support fonts
 *)

signature GC_SERVER =
  sig

    type gc_server

    val mkGCServer : (XDisplay.xdisplay * XProtTypes.drawable_id) -> gc_server

    val acquireGC : gc_server -> {pen : PenRep.pen, used : int}
	  -> XProtTypes.gc_id
    val releaseGC : gc_server -> XProtTypes.gc_id -> unit

    val acquireGCWithFont : gc_server -> {
	    pen : PenRep.pen,
	    used : int,
	    fid : XProtTypes.font_id
	  } -> (XProtTypes.gc_id * XProtTypes.font_id)
    val acquireGCAndSetFont : gc_server -> {
	    pen : PenRep.pen,
	    used : int,
	    fid : XProtTypes.font_id
	  } -> XProtTypes.gc_id
    val releaseGCAndFont : gc_server -> XProtTypes.gc_id -> unit

  end (* GC_SERVER *)

structure GCServer : GC_SERVER =
  struct
    local
      open CML Geometry XProtTypes XRequest PenRep

      val numGCSlots = 23
      val fontGCSlot = 14		(* the slot in a GC for the font *)

      val clipOriginPenSlot = 14	(* the slot in a pen for the clip origin *)
      val clipPenSlot = 15		(* the slot in a pen for the clip mask *)
      val dashOffsetPenSlot = 16	(* the slot in a pen for the dash offset *)
      val dashPenSlot = 17		(* the slot in a pen for the dash list *)

    (* GC request/reply messages.
     * There are two basic requests: acquire and release a GC.  When acquiring
     * a GC, one supplies a pen and bit-vector telling which fields are used by
     * the drawing operation.  For text drawing, there are two forms of acquire
     * request.  AcquireGCWithFont specifies that the font field is needed; the
     * reply will be REPLY_GCWithFont and will specify the current value of the
     * GC's font.  It is the drawing operation's (presumably a DrawText)
     * responsibility to restore the font.  The AcquireGCAndSetFont request
     * requires that the GC have the requested font; and will generate a normal
     * REPLY_GC reply.
     *)
      datatype gc_msg
	= AcquireGC of {pen : pen, used : int}
	| AcquireGCWithFont of {pen : pen, used : int, fid : font_id}
	| AcquireGCAndSetFont of {pen : pen, used : int, fid : font_id}
	| ReleaseGC of gc_id
	| ReleaseGCAndFont of gc_id
      and gc_reply
        = REPLY_GC of gc_id
        | REPLY_GCWithFont of (gc_id * font_id)

      datatype font_sts
	= NoFont		    (* No font has been set yet in this GC *)
	| SomeFont of font_id	    (* There is a font set, but it is not *)
				    (* currently being used. *)
	| UsedFont of		    (* The font is being used with the number of users *)
	    (font_id * int)

      datatype avail_gc = AVAIL of {
	    gc : gc_id,		    (* the X-server GC *)
	    desc : pen,		    (* a descriptor of the values of the GC *)
	    font : font_sts	    (* the current font (if any) *)
	  }
      datatype used_gc = USED of {
	    gc : gc_id,		    (* the X-server GC *)
	    desc : pen,		    (* a descriptor of the values of the GC *)
	    font : font_sts ref,    (* the current font (if any) *)
	    used : int ref,	    (* a bit-mask telling which components of the *)
				    (* GC are being used *)
	    n_users : int ref	    (* the number of clients using the GC, *)
				    (* including those using the font. *)
	  }

(* +DEBUG *)
fun fontSts2s (NoFont) = "NoFont"
  | fontSts2s (SomeFont f) = implode["SomeFont(", XPrint.xidToString f, ")"]
  | fontSts2s (UsedFont(f, n)) = implode[
	"UsedFont(", XPrint.xidToString f, ", ", makestring n, ")"
      ]
fun usedGC2s (USED{gc,desc,font,used,n_users}) = implode[
        "USED{gc=", XPrint.xidToString gc, ", font=", fontSts2s(!font),
	", n_users=", makestring(!n_users), "}"
      ]
(* -DEBUG *)

      val ++ = Bits.orb and & = Bits.andb
      val >> = Bits.rshift and << = Bits.lshift
      infix ++ & << >>

(* +DEBUG 
fun mask2str nbits m = let
      fun f (i, m, l) = if (i = nbits)
	    then implode(rev l)
	    else f (i+1, m >> 1, (if ((m&1) = 0) then "0" else "1") :: l)
      in
        f(0, m, [])
      end
val penMask2str = mask2str PenRep.numPenSlots
val gcMask2str = mask2str numGCSlots
 -DEBUG *)

    (* search a list of used GCs for gcid, and remove if unused *)
      fun findUsedGC (gcid, fontUsed, usedGCs) = let
	    fun f [] = MLXError.impossible "[GCServer: lost used GC]"
	      | f ((x as USED{gc, ...}) :: r) = if (gc <> gcid)
		    then (case (f r)
		     of NONE => NONE
		      | SOME(avail, l) => SOME(avail, x::l))
		    else (case (x, fontUsed)
		     of (USED{n_users = ref 1, desc, font, ...}, false) =>
			  SOME(AVAIL{gc = gc, desc = desc, font = !font}, r)
		      | (USED{
			  n_users = ref 1, desc, font = ref (UsedFont(f, 1)), ...
			}, true) =>
			  SOME(AVAIL{gc = gc, desc = desc, font = SomeFont f}, r)
		      | (USED{n_users as ref n, ...}, false) => (n_users := n-1; NONE)
		      | (USED{
			  n_users as ref n, font as ref(UsedFont(f, 1)), ...
			}, true) => (
			  n_users := n-1; font := (SomeFont f); NONE)
		      | (USED{
			  n_users as ref n, font as ref(UsedFont(f, nf)), ...
			}, true) => (
			  n_users := n-1; font := UsedFont(f, nf-1); NONE)
		      | (gc, _) =>
			  MLXError.impossible (implode [
			      "[GCServer.findUsedGC: bogus used GC; fontUsed = ",
			      makestring fontUsed, ", gc = ", usedGC2s gc, "]"
			    ]))
	    in
	      (f usedGCs)
	    end

      val (penSlotToGCMask, penSlotToGCSlot) = let
	    val l = [
		[0],		(* pen-slot 0:  function *)
		[1],		(* pen-slot 1:  plane mask *)
		[2],		(* pen-slot 2:  foreground *)
		[3],		(* pen-slot 3:  background *)
		[4],		(* pen-slot 4:  line-width *)
		[5],		(* pen-slot 5:  line-style *)
		[6],		(* pen-slot 6:  cap-style *)
		[7],		(* pen-slot 7:  join-style *)
		[8],		(* pen-slot 8:  fill-style *)
		[9],		(* pen-slot 9:  fill-rule *)
		[10],		(* pen-slot 10: tile *)
		[11],		(* pen-slot 11: stipple *)
		[12, 13],	(* pen-slot 12: tile/stipple origin *)
		[15],		(* pen-slot 13: subwindow mode *)
		[17, 18],	(* pen-slot 14: clipping origin *)
		[19],		(* pen-slot 15: clipping mask *)
		[20],		(* pen-slot 16: dash offset *)
		[21],		(* pen-slot 17: dash list *)
		[22]		(* pen-slot 18: arc mode *)
	      ]
	    fun bmsk [] = 0 | bmsk (i::r) = ((1 << i) ++ (bmsk r))
	    in
	      (Vector.vector(map bmsk l), Vector.vector(map hd l))
	    end
      fun penMaskToGCMask penMask = let
	    fun loop (0, _, m) = m
	      | loop (mask, i, m) = if ((mask & 1) = 0)
		  then loop (mask >> 1, i+1, m)
		  else loop (mask >> 1, i+1, m ++ Vector.sub(penSlotToGCMask, i))
	    in
	      loop (penMask, 0, 0)
	    end
		  
    in

    datatype gc_server = GCS of {
	req_ch : gc_msg chan,
	reply_ch : gc_reply chan
      }

  (* Create the GC-server for the given screen *)
    fun mkGCServer (XDisplay.XDPY{conn, nextXId, ...}, drawable) = let
	  val reqCh = channel() and replyCh = channel()
	  val minHitRate = 80 (* want at least 80% of GC requests to be matched *)
	  fun hitRate (hit, miss) = let val tot = hit + miss
		in
		  if (tot = 0) then 100 else ((100 * hit) quot tot)
		end
	  val request = XIo.request conn
	(* map the values of a pen to an X-server GC initialization array.  "dstMask"
	 * specifies which values in the pen are to be mapped.  Assume that all values
	 * are non-default (we copy fields from the screen's default GC for those).
	 *)
	  fun penToGCVals (PEN{vals, ...}, dstMask, font) = let
		val gcVals = Array.array(numGCSlots, NONE)
		fun update (i, v) = Array.update(gcVals, i, v)
		fun initVal (i, PVRep_int v) =
		      update(Vector.sub(penSlotToGCSlot, i), SOME v)
		  | initVal (i, PVRep_point(PT{x, y})) = let
		      val j = Vector.sub(penSlotToGCSlot, i)
		      in
			update(j, SOME x); update(j+1, SOME y)
		      end
		  | initVal (i, PVRep_pixmap(XID id)) =
		      update(Vector.sub(penSlotToGCSlot, i), SOME id)
		  | initVal _ = ()
		fun initVals (0, _) = ()
		  | initVals (m, i) = (if ((m & 1) <> 0)
			then initVal(i, Vector.sub(vals, i))
			else ();
		    initVals (m >> 1, i+1))
		in
		  case font
		   of NONE => () | SOME(XID fid) => update(fontGCSlot, SOME fid);
		  initVals(dstMask, 0);
		  {
		    vals = VALS gcVals,
		    clip_rects = if ((dstMask & (1 << clipPenSlot)) = 0)
			then NONE
			else (case Vector.sub(vals, clipPenSlot)
			 of PVRep_rects rects => (
			      SOME(Vector.sub(vals, clipOriginPenSlot), rects))
			  | _ => NONE),
		    dashes = if ((dstMask & (1 << dashPenSlot)) = 0)
			then NONE
			else (case Vector.sub(vals, dashPenSlot)
			 of PVRep_dashes dashes => (
			      SOME(Vector.sub(vals, dashOffsetPenSlot), dashes))
			  | _ => NONE)
		  }
		end (* penToGCVals *)
	  fun setDashes (_, NONE) = ()
	    | setDashes (id, SOME(PVRep_int offset, d)) = request (
		encodeSetDashes {gc = id, dash_offset = offset, dashes = d})
	    | setDashes (id, SOME(_, d)) = request (
		encodeSetDashes {gc = id, dash_offset = 0, dashes = d})
	  fun setClipRects (_, NONE) = ()
	    | setClipRects (id, SOME(PVRep_point pt, (order, rects))) = request (
		encodeSetClipRectangles{
		    gc = id, clip_origin = pt, ordering = order, rects = rects
		  })
	    | setClipRects (id, SOME(_, (order, rects))) = request (
		encodeSetClipRectangles{
		    gc = id, clip_origin = originPt,
		    ordering = order, rects = rects
		  })
	(* Set the font of a GC *)
	  fun setFont (gcId, XID fid) = let
		val vals = Array.array(numGCSlots, NONE)
		in
		  Array.update(vals, fontGCSlot, SOME fid);
		  request (encodeChangeGC{gc = gcId, vals = VALS vals})
		end
	(* Create a new server GC, which is, by definition, used. *)
	  fun newGC (pen as PEN{mask, ...}, usedMask, font) = let
		val {vals, dashes, clip_rects} = penToGCVals(pen, mask, font)
		val gcid = nextXId()
		in
		  request (encodeCreateGC {gc=gcid, drawable=drawable, vals=vals});
		  setDashes (gcid, dashes);
		  setClipRects (gcid, clip_rects);
		  USED{
		      gc = gcid, desc = pen, font = ref(
			case font of NONE => NoFont | (SOME f) => UsedFont(f, 1)),
		      used = ref usedMask, n_users = ref 1
		    }
		end
	  val defaultGC as USED{gc = dfltGCId, ...} = newGC(defaultPen, 0x7FFFFF, NONE)
	(* update a server GC so that it agrees with the given pen on the used values *)
	  fun changeGC (
		AVAIL{gc=gcId, font=curFont, ...}, pen as PEN{mask, ...},
		usedMask, newFont
	      ) = let
		val nonDefaultMask = mask & usedMask
		val defaultMask = (Bits.notb mask) & usedMask
		val (differentFont, font) = case (curFont, newFont)
		     of (_, NONE) => (false, NoFont)
		      | (NoFont, SOME fid) => (true, UsedFont(fid, 1))
		      | (SomeFont fid1, SOME fid2) =>
			  ((fid1 <> fid2), UsedFont(fid2, 1))
		      | (UsedFont _, _) =>
			  MLXError.impossible "[GCServer: used font in avail gc]"
		in
		  if (defaultMask <> 0)
		    then request (encodeCopyGC{
			src = dfltGCId, dst = gcId,
			mask = VALMASK(penMaskToGCMask defaultMask)
		      })
		    else ();
		  if ((nonDefaultMask <> 0) orelse differentFont)
		    then let
		      val {vals, dashes, clip_rects} = penToGCVals(pen, mask, newFont)
		      in
			request (encodeChangeGC{gc = gcId, vals = vals});
			setDashes (gcId, dashes);
			setClipRects (gcId, clip_rects)
		      end
		    else ();
		  USED{
		      gc = gcId, desc = pen, font = ref font,
		      used = ref usedMask, n_users = ref 1
		    }
		end
	(* search a list of used GCs for one that matches the given pen *)
	  fun matchUsedGC (pen, usedMask, font, usedGCs) = let
(** NOTE: there may be used components in pen that are not used in arg, but that
 ** are defined differently.  We could still use arg, but we'll have to update it.
 ** The test for an approx. match would be:
 **		    if penMatch(m & usedMask, pen, desc)
 **)
	        val match = case font
		     of NONE => (fn (USED{desc, ...}) => penMatch(usedMask, pen, desc))
		      | (SOME f) => let
			  fun match (USED{desc, font = ref(UsedFont(f', _)), ...}) = (
				(f = f') andalso penMatch(usedMask, pen, desc))
			    | match (USED{desc, ...}) = penMatch(usedMask, pen, desc)
			  in
			    match
			  end
		fun f [] = NONE
		  | f (arg::r) = if (match arg)
			then let
			  val USED{gc, n_users, used, ...} = arg
			  in
			    n_users := !n_users + 1;
			    used := ((!used) ++ usedMask);
			    SOME arg
			  end
			else (f r)
		in
		  f usedGCs
		end
	(* search the list of available GCs for a match.  If none is found, then take
	 * the last one and modify it to work.  If the list is empty, then create a
	 * new GC. *)
	  fun matchAvailGC (hit, miss, pen, usedMask, font, availGCs) = let
		fun revappend ([], l) = l
		  | revappend (x::r, l) = revappend (r, x::l)
		val (match, mkUsed) = (case font
		     of NONE => let
			  fun match (AVAIL{desc, ...}) = penMatch(usedMask, pen, desc)
			  fun mkUsed (AVAIL{gc, desc, font}) = USED{
				  gc = gc, desc = desc, font = ref font,
				  used = ref usedMask, n_users = ref 1
				}
			  in
			    (match, mkUsed)
			  end
		      | (SOME fid) => let
			  fun match (AVAIL{desc, font = NoFont, ...}) = false
			    | match (AVAIL{desc, font = SomeFont f, ...}) = (
				(f = fid) andalso penMatch(usedMask, pen, desc))
			    | match (AVAIL{font = (UsedFont _), ...}) =
				MLXError.impossible "[GCServer: used font in avail gc]"
			  fun mkUsed (AVAIL{gc, desc, ...}) = USED{
				  gc = gc, desc = desc, font = ref(UsedFont(fid, 1)),
				  used = ref usedMask, n_users = ref 1
				}
			  in
			    (match, mkUsed)
			  end)
		fun f ([], l) = (newGC(pen, usedMask, font), 0, 0, revappend(l, []))
		  | f ([last as AVAIL{gc, ...}], l) = if (match last)
			then (mkUsed last, hit+1, miss, revappend(l, []))
		      else if (hitRate(hit, miss) < minHitRate)
			then (newGC(pen, usedMask, font), 0, 0, revappend(l, [last]))
			else (
			    changeGC(last, pen, usedMask, font),
			    hit, miss+1, revappend(l, [])
			  )
		  | f (x :: r, l) = if (match x)
		      then (mkUsed x, hit+1, miss, revappend(l, r))
		      else f(r, x::l)
		in
		  f (availGCs, [])
		end
	  fun server (hit, miss, inuse, avail) = let
		val _ = ()
		in
		  case (accept reqCh)
		   of AcquireGC{pen, used=usedMask} => (
			case (matchUsedGC (pen, usedMask, NONE, inuse))
			 of SOME(USED{gc, ...}) => (
			      send(replyCh, REPLY_GC gc);
			      server(hit+1, miss, inuse, avail))
			  | NONE => let
			      val (x as USED{gc, ...}, h, m, a) =
				    matchAvailGC(hit, miss, pen, usedMask, NONE, avail)
			      in
				send(replyCh, REPLY_GC gc);
				server(h, m, x :: inuse, a)
			      end)
		    | AcquireGCWithFont{pen, used=usedMask, fid=fId} => (
			case (matchUsedGC (pen, usedMask, NONE, inuse))
			 of SOME(USED{gc, font as (ref NoFont), ...}) => (
			      setFont(gc, fId);
			      font := UsedFont(fId, 1);
			      send(replyCh, REPLY_GCWithFont(gc, fId));
			      server(hit+1, miss, inuse, avail))
			  | SOME(USED{gc, font as (ref(SomeFont f)), ...}) => (
			      if (f <> fId)
				then (setFont(gc, fId); font := UsedFont(fId, 1))
				else (font := UsedFont(fId, 1));
			      send(replyCh, REPLY_GCWithFont(gc, fId));
			      server(hit+1, miss, inuse, avail))
			  | SOME(USED{gc, font as (ref(UsedFont(f, n))), ...}) => (
			      font := UsedFont(f, n+1);
			      send(replyCh, REPLY_GCWithFont(gc, f));
			      server(hit+1, miss, inuse, avail))
			  | NONE => let
			      val (x as USED{gc, ...}, h, m, a) =
				    matchAvailGC(
				      hit, miss, pen, usedMask, SOME fId, avail)
			      in
				send(replyCh, REPLY_GCWithFont(gc, fId));
				server(h, m, x :: inuse, a)
			      end)
		    | AcquireGCAndSetFont{pen, used=usedMask, fid=fId} =>(
			case (matchUsedGC (pen, usedMask, SOME fId, inuse))
			 of SOME(USED{gc, font as (ref NoFont), ...}) => (
			      setFont(gc, fId);
			      font := UsedFont(fId, 1);
			      send(replyCh, REPLY_GC gc);
			      server(hit+1, miss, inuse, avail))
			  | SOME(USED{gc, font as (ref(SomeFont f)), ...}) => (
			      if (f <> fId) then setFont(gc, fId) else ();
			      font := UsedFont(fId, 1);
			      send(replyCh, REPLY_GC gc);
			      server(hit+1, miss, inuse, avail))
			  | SOME(USED{gc, font as (ref(UsedFont(f, n))), ...}) => (
			      font := UsedFont(f, n+1);  (* NOTE: f = fId! *)
			      send(replyCh, REPLY_GC gc);
			      server(hit+1, miss, inuse, avail))
			  | NONE => let
			      val (x as USED{gc, ...}, h, m, a) =
				    matchAvailGC(
				      hit, miss, pen, usedMask, SOME fId, avail)
			      in
				send(replyCh, REPLY_GC gc);
				server(h, m, x :: inuse, a)
			      end)
		    | ReleaseGC id => (case findUsedGC(id, false, inuse)
		       of NONE => server(hit, miss, inuse, avail)
			| SOME(x, l) => server(hit, miss, l, x :: avail))
		    | ReleaseGCAndFont id => (case findUsedGC(id, true, inuse)
		       of NONE => server(hit, miss, inuse, avail)
			| SOME(x, l) => server(hit, miss, l, x :: avail))
		end
		
	  in
	    XDebug.xspawn("GC-Server", fn () => server(0, 0, [defaultGC], []));
	    GCS{req_ch = reqCh, reply_ch = replyCh}
	  end (* mkGCServer *)

    fun acquireFn msgKind (GCS{req_ch, reply_ch}) arg = (
	  send(req_ch, msgKind arg);
	  case (accept reply_ch)
	   of REPLY_GC id => id
	    | _ => MLXError.impossible "[GCServer.acquireFn: bad reply]")

    val acquireGC = acquireFn AcquireGC
    val acquireGCAndSetFont = acquireFn AcquireGCAndSetFont
    
    fun acquireGCWithFont (GCS{req_ch, reply_ch}) arg = (
	  send(req_ch, AcquireGCWithFont arg);
	  case (accept reply_ch)
	   of REPLY_GCWithFont arg => arg
	    | _ => MLXError.impossible "[GCServer.acquireGCWithFont: bad reply]")

    fun releaseGC (GCS{req_ch, ...}) gcid = send(req_ch, ReleaseGC gcid)
    fun releaseGCAndFont (GCS{req_ch, ...}) arg = send(req_ch, ReleaseGCAndFont arg)

(* +DEBUG *)
local
  fun pr (s, gc) = XDebug.trace(XDebug.gcTM, fn () => [
	  tidToString(getTid()), " ", s, ": gc = ", XPrint.xidToString gc, "\n"
	])
in
val acquireGC = fn a =>
      (fn arg => let val gc = acquireGC a arg in pr("acquireGC", gc); gc end)
val acquireGCAndSetFont = fn a =>
      (fn arg => let val gc = acquireGCAndSetFont a arg
	in pr("acquireGCAndSetFont", gc); gc end)
val acquireGCWithFont = fn a =>
      (fn arg => let val res as (gc, _) = acquireGCWithFont a arg
	in pr("acquireGCWithFont", gc); res end)
val releaseGC = fn a => (fn gc => (pr("releaseGC", gc); releaseGC a gc))
val releaseGCAndFont = fn a =>
      (fn gc => (pr("releaseGCAndFont", gc); releaseGCAndFont a gc))
end
(* -DEBUG *)

    end (* local *)
  end (* GCServer *)
