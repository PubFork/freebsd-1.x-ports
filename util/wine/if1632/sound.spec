# $Id: sound.spec,v 1.1 1994/02/24 08:05:34 hsu Exp $
#
name	sound
id	7
length	20

1    pascal  OPENSOUND() OpenSound()
2    pascal  CLOSESOUND() CloseSound()
3    pascal  SETVOICEQUEUESIZE(word word) SetVoiceQueueSize(1 2)
4    pascal  SETVOICENOTE(word word word word) SetVoiceNote(1 2 3 4)
5    pascal  SETVOICEACCENT(word word word word word) 
		       SetVoiceAccent(1 2 3 4 5)
6    pascal  SETVOICEENVELOPE(word word word) SetVoiceEnvelope(1 2 3)
7    pascal  SETSOUNDNOISE(word word) SetSoundNoise(1 2)
8    pascal  SETVOICESOUND(word long word) SetVoiceSound(1 2 3)
9    pascal  STARTSOUND() StartSound()
10   pascal  STOPSOUND() StopSound()
