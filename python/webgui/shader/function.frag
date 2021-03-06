varying vec3 p_;
varying vec3 normal_;
varying vec3 value_;

uniform bool render_depth;

void main()
{

  if(function_mode == 4.0)
  {
    gl_FragColor = vec4(value_, 1.0);
    return;
  }

  if( isBehindClippingPlane(p_) )
    discard;

    if (render_depth) {
    gl_FragColor = getPositionAsColor(p_);
    return;
  }


  vec3 norm = normal_;
  bool inside = false;
#ifndef SKIP_FACE_CHECK
  if (gl_FrontFacing == false) {
    norm = (-1.0)*normal_;
    inside = true;
  }
#endif // SKIP_FACE_CHECK

#ifdef NO_FUNCTION_VALUES
  vec4 color = vec4(.7,.7,.7,1);
#else
  vec4 color = getColor(GetValue(value_));
#endif

  gl_FragColor = calcLight( color, p_, norm, inside);
}
