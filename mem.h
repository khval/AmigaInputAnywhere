
#define allocShared( x ) AllocVecTags( sizeof(struct InputEvent), AVT_Type, MEMF_SHARED,  AVT_ClearWithValue, 0, TAG_END )
#define allocPrivate( x ) AllocVecTags( sizeof(struct InputEvent), AVT_Type, MEMF_PRIVATE,  AVT_ClearWithValue, 0, TAG_END )
