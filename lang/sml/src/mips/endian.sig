signature ENDIAN = 
    sig 
	val order_real: string -> string
	val low_order_offset: int
	val wordLayout: int * int -> (int * int * int * int)
    end

