#pragma once


#include "Renderable.h"

class EngineExport VertexSpline
{
public:
	VertexSpline( void );

	~VertexSpline( void );

    /** Adds a control point to the end of the spline. */
    void AddPoint(const Vector3Df& p);

    /** Gets the detail of one of the control points of the spline. */
    const Vector3Df& GetPoint(unsigned short index) const;

    /** Gets the number of control points in the spline. */
    unsigned short GetNumPoints(void) const;

    /** Clears all the points in the spline. */
    void Clear(void);

    /** Updates a single point in the spline. 
    @remarks
        This point must already exist in the spline.
    */
    void UpdatePoint(unsigned short index, const Vector3Df& value);

    /** Returns an interpolated point based on a parametric value over the whole series.
    @remarks
        Given a t value between 0 and 1 representing the parametric distance along the
        whole length of the spline, this method returns an interpolated point.
    @param t Parametric value.
    */
    Vector3Df Interpolate(float t);

    /** Interpolates a single segment of the spline given a parametric value.
    @param fromIndex The point index to treat as t=0. fromIndex + 1 is deemed to be t=1
    @param t Parametric value
    */
    Vector3Df Interpolate(unsigned int fromIndex, float t);


    /** Tells the spline whether it should automatically calculate tangents on demand
        as points are added.
    @remarks
        The spline calculates tangents at each point automatically based on the input points.
        Normally it does this every time a point changes. However, if you have a lot of points
        to add in one go, you probably don't want to incur this overhead and would prefer to 
        defer the calculation until you are finished setting all the points. You can do this
        by calling this method with a parameter of 'false'. Just remember to manually call 
        the recalcTangents method when you are done.
    @param autoCalc If true, tangents are calculated for you whenever a point changes. If false, 
        you must call reclacTangents to recalculate them when it best suits.
    */
    void SetAutoCalculate(bool autoCalc);

    /** Recalculates the tangents associated with this spline. 
    @remarks
        If you tell the spline not to update on demand by calling setAutoCalculate(false)
        then you must call this after completing your updates to the spline points.
    */
    void RecalcTangents(void);

protected:
    bool					m_bAutoCalc;
	AIRVector<Vector3Df>	m_vtPoints;
    AIRVector<Vector3Df>	m_vtTangents;
    Matrix4f				m_matCoeffs; // Matrix of coefficients 


};//! end class VertexSpline







/** This class interpolates orientations (rotations) along a spline using 
    derivatives of quaternions.
@remarks
    Like the SimpleSpline class, this class is about interpolating values 
    smoothly over a spline. Whilst SimpleSpline deals with positions (the normal
    sense we think about splines), this class interpolates orientations. The
    theory is identical, except we're now in 4-dimensional space instead of 3.
@par
    In positional splines, we use the points and tangents on those points to generate
    control points for the spline. In this case, we use quaternions and derivatives
    of the quaternions (i.e. the rate and direction of change at each point). This is the
    same as SimpleSpline since a tangent is a derivative of a position. We effectively 
    generate an extra quaternion in between each actual quaternion which when take with 
    the original quaternion forms the 'tangent' of that quaternion.
*/
class EngineExport RotationalSpline
{
public:
    RotationalSpline();
    ~RotationalSpline();

    /** Adds a control point to the end of the spline. */
    void AddPoint(const Quaternion& p);

    /** Gets the detail of one of the control points of the spline. */
    const Quaternion& GetPoint(unsigned short index) const;

    /** Gets the number of control points in the spline. */
    unsigned short GetNumPoints(void) const;

    /** Clears all the points in the spline. */
    void Clear(void);

    /** Updates a single point in the spline. 
    @remarks
        This point must already exist in the spline.
    */
    void UpdatePoint(unsigned short index, const Quaternion& value);

    /** Returns an interpolated point based on a parametric value over the whole series.
    @remarks
        Given a t value between 0 and 1 representing the parametric distance along the
        whole length of the spline, this method returns an interpolated point.
    @param t Parametric value.
	@param useShortestPath Defines if rotation should take the shortest possible path
    */
    Quaternion Interpolate(float t, bool useShortestPath=true);

    /** Interpolates a single segment of the spline given a parametric value.
    @param fromIndex The point index to treat as t=0. fromIndex + 1 is deemed to be t=1
    @param t Parametric value
	@param useShortestPath Defines if rotation should take the shortest possible path
    */
    Quaternion Interpolate(unsigned int fromIndex, float t, bool useShortestPath=true);

    /** Tells the spline whether it should automatically calculate tangents on demand
        as points are added.
    @remarks
        The spline calculates tangents at each point automatically based on the input points.
        Normally it does this every time a point changes. However, if you have a lot of points
        to add in one go, you probably don't want to incur this overhead and would prefer to 
        defer the calculation until you are finished setting all the points. You can do this
        by calling this method with a parameter of 'false'. Just remember to manually call 
        the recalcTangents method when you are done.
    @param autoCalc If true, tangents are calculated for you whenever a point changes. If false, 
        you must call reclacTangents to recalculate them when it best suits.
    */
    void SetAutoCalculate(bool autoCalc);

    /** Recalculates the tangents associated with this spline. 
    @remarks
        If you tell the spline not to update on demand by calling setAutoCalculate(false)
        then you must call this after completing your updates to the spline points.
    */
    void RecalcTangents(void);

protected:
    bool					m_bAutoCalc;
	AIRVector<Quaternion> m_vtPoints;
	AIRVector<Quaternion> m_vtTangents;
};



class EngineExport SplineObject : public Renderable
{
public:
	SplineObject();

	virtual ~SplineObject( void );

	virtual bool LoadFromFile( const AIRString& szFile );

	VertexSpline* GetSpline( void );

	void SetMaxSegmentCount( int nMaxSC );

	void SetSegmentCount( int nSC );

	void SetColor( const SColor& color );

	void CreateSplineVertexBuffer( void );

	void GenLine( void );

	virtual void Update( float fTimeElapsed );

	virtual void PreRender(CCamera* pCamera);

	//virtual void Render(CCamera* pCamera);

	virtual void Clear( void );

protected:
    virtual void _GetPrimitive();
	IVertexBuffer*	m_pVB;
	VertexSpline	m_Spline;
	int				m_nSegmentCount;
	int				m_nMaxSegmentCount;
	SColor			m_Color;
};
