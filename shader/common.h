#ifdef GLSLES

#ifdef FRAGMENT_SHADER
precision highp float;
#endif

#else

/* Desktop GLSL doesn't know about these */
#define highp
#define mediump
#define lowp

#endif
