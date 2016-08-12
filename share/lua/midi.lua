
midi = {}

function midi.message( b1, b2, b3 )
	return bit32.bor( bit32.lshift( b1, 0 ), bit32.lshift( b2, 8 ), bit32.lshift( b3, 16 ) )
end

function midi.channelmessage( pMessage, pChannel )
	return bit32.bor( pMessage, pChannel - 1 )
end

function midi.noteon( pChannel, pNote, pVelocity )
	return midi.message( midi.channelmessage( 0x90, pChannel ), pNote, pVelocity )
end

return midi
