(* str-edit.sml
 *
 * COPYRIGHT (c) 1991 by AT&T Bell Laboratories  See COPYRIGHT file for details.
 *
 * String edit widget.
 *)

signature STREDIT = 
  sig

    structure W : WIDGET

    type str_edit
  
    val mkStrEdit : W.root -> {
      foregrnd : W.EXB.color option,
      backgrnd : W.EXB.color option,
      initval : string,
      minlen : int
    } -> str_edit

    val setString : str_edit -> string -> unit
    val getString : str_edit -> string
    val shiftWin  : str_edit -> int -> unit
    val widgetOf : str_edit -> W.widget

  end (* STREDIT *)

structure StrEdit : STREDIT = struct

  structure EXB = EXeneBase
  structure W = Widget

  open CML Geometry EXeneBase EXeneWin Interact Drawing Widget
  open ExtStr TxtWin

  datatype rqst =
    GetString
  | GetBounds
  | SetString of string
  | ShiftWin of int
  | DoRealize of {
      env : in_env,
      win : window,
      sz : size
    }
  datatype reply =
    Bnds of bounds
  | Str of string

  datatype input = 
    MoveC of int
  | Insert of string
  | Erase
  | Kill

  fun keyP (k, inputc) = let
    val lookup = lookupString defaultTranslation
    fun isErase c = c = ord("\^H")
    fun isKill c = c = ord("\^X")

    fun doChars s = let
      val slen = size s
      fun doChar i =
        if i = slen then ()
        else let
          val c = ordof(s,i)
        in
(* NOTE: 0xa0 = (ord " " + 128) *)
          if ((c >= ord " ") andalso ((c <= ord "~") orelse (c >= 0xa0)))
	    then (send (inputc, Insert (chr c)); doChar (i+1))
          else if isErase c
	    then (send (inputc, Erase); doChar (i+1))
          else if isKill c
	    then (send (inputc, Kill); doChar (i+1))
            else doChar(i+1)
        end
    in
      doChar 0
    end

    fun loop () =
      case msgBodyOf (sync k) of 
        KEY_Press key => (
          doChars (lookup key) handle KeysymNotFound => ();
          loop ()
        )
      | _ => loop ()
  in
    loop ()
  end

  fun mseP (m, mchan, pttopos) = let
    val waitUp = whileMouseState mbutSomeSet
    val mevt = wrap (m, fn evt => msgBodyOf evt)

    fun loop () =
      case msgBodyOf (sync m) of 
        MOUSE_FirstDown {pt,but,...} => (
          send (mchan, MoveC (pttopos pt));
          waitUp (mkButState [but], mevt);
          loop ()
        )
      | _ => loop ()
  in
    loop ()
  end

  val dfltMinchars = 4

  datatype str_edit = StrEdit of (widget * rqst chan * reply chan)

  fun mkStrEdit root {
    foregrnd : color option,
    backgrnd : color option,
    initval : string,
    minlen : int
  } =
  let

    val minchars = max(minlen, dfltMinchars)
    val dpy = displayOf root
    val (bndf, pttopos, realize) = mkTxtWin root (foregrnd, backgrnd)
    val reqChan = channel () and repChan = channel ()
    val inputc = channel ()
    val SIZE{wid=minlen,...} = bndf minchars

    fun getbnds slen = let
      val SIZE{wid,ht} = bndf (max(minchars,slen))
      val x_dim = DIM{base=0, incr=1, min=minlen, nat=wid, max=NONE}
    in
      {x_dim=x_dim, y_dim= fixDim ht}
    end

    fun initOff (slen, winlen) =
      if slen <= winlen then 0
      else slen - (winlen div 2)

    fun realizeStrEdit {env=InEnv{m,k,ci,co}, win, sz} initStr = let
      val my_win = win
      val {set_size, set_cur_pos, set_cursor, 
        insert, reset, deletec} = realize (my_win, sz)

      fun main winLen me = let

        fun isCurVisible (_,pos,woff) =
          (woff <= pos) andalso (pos <= woff+winLen)

        fun redraw (me as (str,pos,woff)) = (
          reset ();
          insert (es_subs(str,woff,winLen));
          if isCurVisible me then (
            set_cur_pos (pos - woff);
            set_cursor true
          )
          else ()
        )

        fun rightShift (v, me as (str,pos,woff)) =
          if v = 0 then me
          else let
            val me' = (str, pos, woff + v)
          in
            if v = 1 then (
              set_cursor false;
              set_cur_pos 1;
              deletec (es_subs(str,woff+winLen,1) handle BadIndex _ => "");
              if isCurVisible me' then (
                set_cur_pos (pos - woff - 1);
                set_cursor true
              )
              else ()
            )
            else redraw me';
            me'
          end

        fun leftShift (v, me as (str,pos,woff)) =
          if v = 0 then me
          else let
            val me' = (str, pos, woff - v)
          in
            if v = 1 then (
              set_cursor false;
              set_cur_pos 0;
              insert (es_subs(str,woff-1,1));
              if isCurVisible me' then (
                set_cur_pos (pos - woff + 1);
                set_cursor true
              )
              else ()
            )
            else redraw me';
            me'
          end

        fun shiftWin (v, me as (str,_,woff)) =
          if v <= 0 then (
            if woff = 0 then ringBell dpy 0 else ();
            leftShift (min(~v,woff),me)
          )
          else rightShift (min(v,(es_len str)-woff), me)

        fun mkCurVis (me as (str, pos, woff)) =
          if isCurVisible me then me
          else if pos < woff then 
            leftShift (woff-max(0,pos - (winLen div 2)),me)
          else
            rightShift (pos - (winLen div 2) - woff,me)

        fun insertc (c, me as (str, pos, woff)) =
          if pos - woff = winLen then
            let
              val woff' = max(pos-1,pos+1-winLen)
              val me' = (es_ins (str,pos,c),pos+1,woff')
            in
              if es_len str = winLen then sync(co CO_ResizeReq) else ();
              redraw me';
              me'
            end
          else (
            if es_len str = winLen then sync(co CO_ResizeReq) else ();
            insert c;
            (es_ins (str, pos, c), pos+1,woff)
          )

        fun erasec (me as (str, pos, woff)) =
          if pos = 0 then (
            ringBell dpy 0;
            me
          )
          else if pos = woff andalso woff > 0 then
            let
              val woff' = max(0,pos+1-winLen)
              val me' = (es_del (str,pos),pos-1,woff')
            in
              if es_len str > winLen then sync(co CO_ResizeReq) else ();
              redraw me';
              me'
            end
          else (
            if (es_len str <= (winLen+3)) andalso (winLen < es_len str) then 
              sync(co CO_ResizeReq) 
            else ();
            deletec (es_subs(str,woff+winLen,1) handle BadIndex _ => "");
            (es_del(str,pos),pos-1,woff)
          )

        fun kill (str,_,_) = let
          val me' = (mkExtStr "", 0, 0)
        in
          if es_len str > winLen then sync(co CO_ResizeReq) else ();
          redraw me';
          me'
        end

        fun handleInput (MoveC p,(str,pos,woff)) =
          let
            val pos' = min(es_len str,woff+p)
          in
            if pos <> pos' then (
              set_cur_pos (pos' - woff);
              set_cursor true
            )
            else ();
            (str,pos',woff)
          end
          | handleInput (Insert s,me) = insertc(s, mkCurVis me)
          | handleInput (Erase,me) = erasec (mkCurVis me)
          | handleInput (Kill,me) = kill me

        fun handleCI (CI_Resize (RECT{wid,ht,...}), (str,pos,_)) =
            initMain (SIZE{wid=wid,ht=ht},str,pos)
          | handleCI (CI_Redraw _, me) = (redraw me; me)
          | handleCI (_,me) = me

        fun handleReq (GetString,me as (str,_,_)) = 
            (send(repChan, Str (es_gets str));me)
          | handleReq (ShiftWin arg,me as (str,_,_)) = 
            shiftWin (arg, me)
          | handleReq (GetBounds,me as (str,_,_)) = 
            (send(repChan, Bnds (getbnds (es_len str)));me)
          | handleReq (SetString s,_) =
            let
              val slen = size s
              val me' = (mkExtStr s, slen, initOff(slen, winLen))
            in
              sync(co CO_ResizeReq);
              redraw me';
              me'
            end
          | handleReq (DoRealize _,me) = me

        fun loop me =
          loop (select [
            wrap (ci, fn evt => handleCI (msgBodyOf evt,me)),
            wrap (receive reqChan, fn evt => handleReq (evt,me)),
            wrap (receive inputc, fn evt => handleInput (evt,me))
          ])

      in
        loop me
      end

      and initMain (sz,str,pos) = let
        val winlen = set_size sz
      in
        main winlen (str, pos, initOff(pos,winlen))
      end

    in
      spawn (fn () => mseP (m, inputc, pttopos));
      spawn (fn () => keyP (k, inputc));
      initMain (sz, mkExtStr initStr, size initStr)
    end

    fun initLoop str =
      case accept reqChan of
        GetString => (send(repChan, Str str); initLoop str)
      | GetBounds => (send(repChan, Bnds (getbnds (size str))); initLoop str)
      | SetString str' => initLoop str'
      | DoRealize arg => realizeStrEdit arg str
      | ShiftWin _ => initLoop str

  in
    spawn (fn () => (initLoop initval;()));
    StrEdit (
      mkWidget{
        root=root,
        boundsOf = fn () => (
          send (reqChan, GetBounds);
          case accept repChan of
            Bnds b => b
          | Str _ => raise LibBase.Impossible "StrEdit.mkStrEdit"
        ),
        realize = (fn arg => (send (reqChan, DoRealize arg)))
      },
      reqChan,
      repChan
    )
  end

  fun widgetOf (StrEdit(widget,_,_)) = widget

  fun setString (StrEdit(_,reqc,_)) arg = (send (reqc, SetString arg))

  fun shiftWin (StrEdit(_,reqc,_)) arg = (send (reqc, ShiftWin arg))

  fun getString (StrEdit(_,reqc,repc)) = (
    send (reqc, GetString);
    case accept repc of
      Bnds _ => raise LibBase.Impossible "StrEdit.getString"
    | Str s => s
  )

end (* StrEdit *)

