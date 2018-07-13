#ifndef TF2DPRIMITIVE_H
#define TF2DPRIMITIVE_H

#include "Rendering/Vector.h"

/**
 * Abstract base class for all primitives that are used in 2D transfer functions.
 */
class TF2DPrimitive {
public:
    // Standard constructor
    TF2DPrimitive();

    // Constructor
    TF2DPrimitive(Vector4f col, float scaleFactor);

    // Destructor
    virtual ~TF2DPrimitive();

    // Sets the scaleFactor of the primitive to the given value. The y coordinates
    // of the primitive are adjusted according to the scaleFactor.
    virtual void setScaleFactor(float scaleFactor) = 0;	
	
	// Returns the scale factor
	float getScaleFactor() const;

    // Paints the primitive.
    virtual void paint() = 0;

    // Paints the primitive for display in an editor. An outline and control points are added.
    virtual void paintInEditor() = 0;

    // Paints the primitive to decide whether it was selected or not.
    // Therefor the color that is used in painting is set to [id, 123, 123].
    virtual void paintForSelection(unsigned char id) = 0;

    // Paints a control point at the given position.
    virtual void paintControlPoint(const Vector2f& v);

    // Sets the color of the primitive to the given value.
    void setColor(const Vector4f& c);

    // Returns the current color of the primitive.
    Vector4f getColor() const;

    // Sets the fuzziness of the primitive to the given value. With increasing fuzziness the
    // primitives get an alphagradient towards their border.
    void setFuzziness(float f);

    // Returns the current fuzziness of the primitive.
    float getFuzziness() const;

    // Returns the size of a control point.
    float getControlPointSize() const;

    // Returns the distance between pos and the closest control point.
    virtual float getClosestControlPointDist(Vector2f pos) = 0;

    // Returns true if a control point is under the mouse cursor and grabs this control point.
    // False is returned when there is no control point under the mouse cursor.
    virtual bool selectControlPoint(Vector2f pos) = 0;

    // Moves the primitive by the given offset. If a control point is grabbed only the control
    // point is moved.
    virtual bool move(Vector2f offset) = 0;

    // Sets the selection state of this primitive to true. Furthermore is tested whether the
    // mouse cursor is over a control point.
    virtual void select(Vector2f pos);

    // Sets the selection state of the primitive to false.
    virtual void deselect();

	// Load the parameters of the primitve
	virtual void load(FILE *fp);

	// Save the parameters of the primitive
	virtual void save(FILE *fp);

protected:
    Vector4f color;		///< color of the primitive
    bool selected;		///< indicates whether the primitive is selected or not
    float fuzziness;	///< fuzziness of the primitive
    float cpSize;		///< size of a control point
    int grabbed;		///< number of the grabbed control point. -1 when no control point is grabbed
    float scaleFactor;	///< scaling of the primitive coordinates in y direction
};

// ----------------------------------------------------------------------------

/**
 * A quad primitive. It consists of 4 vertices that can be moved independently.
 */
class TransFuncQuad : public TF2DPrimitive {
public:
    // Constructor
    TransFuncQuad();

    // Constructor
    TransFuncQuad(Vector2f center, float size, Vector4f col, float scaleFactor);

    // Destructor
    ~TransFuncQuad();

    // Sets the scaleFactor of the primitive to the given value. The y coordinates
    // of the primitive are adjusted according to the scaleFactor.
    void setScaleFactor(float scaleFactor);

    // Paints the quad. The fuzziness factor is obeyed.
    void paint();

    // Paints the quad for selection purposes. Therefor the color used in painting is set
    // to [id, 123, 123].
    void paintForSelection(unsigned char id);

    // Paints the quad for display in an editor. An outline and control points are added.
    void paintInEditor();

    // Returns the distance between pos and the closest control point.
    float getClosestControlPointDist(Vector2f pos);

    // Grabs a control point when the mouse cursor is over one. Returns true when a control
    // point is grabbed and false otherwise.
    bool selectControlPoint(Vector2f pos);

    // Moves the 4 vertices by the given offset. Returns true when the moved vertices
    // are lying in the range [0,1] and false otherwise.
    bool move(Vector2f offset);

	// Load the parameters of the primitve
	void load(FILE *fp);

	// Save the parameters of the primitive
	void save(FILE *fp);

protected:
    Vector2f coords[4];		///< the coordinates of the 4 vertices
    bool scaleCoords;		///< indicates whether the coordinates must be scaled in paint() or not
};

// ----------------------------------------------------------------------------

/**
 * A banana primitive. It consists of 4 vertices that are connected by 2 splines.
 * The control points are arranged in the following way.
 *       1
 *
 *       2
 * 0          3
 * The first spline starts at point 0, goes through 1 and ends in 3.
 * The second spline starts at point 0, goes through 2 and ends in 3.
 */
class TransFuncBanana : public TF2DPrimitive {
public:
    // Constructor
    TransFuncBanana();

    // Constructor
    TransFuncBanana(Vector2f a, Vector2f b1, Vector2f b2, Vector2f c, Vector4f col, float scaleFactor);

	// Destructor
    ~TransFuncBanana();

    // Sets the scaleFactor of the primitive to the given value. The y coordinates
    // of the primitive are adjusted according to the scaleFactor.
    void setScaleFactor(float scaleFactor);

    // Paints the banana. The fuzziness is obeyed.
    void paint();

    // Paints the banana for selection purposes. Therefor the color used in painting is set
    // to [id, 123, 123].
    void paintForSelection(unsigned char id);

    // Paints the banana for display in an editor. An outline and control points are added.
    void paintInEditor();

    // Returns the distance between pos and the closest control point.
    float getClosestControlPointDist(Vector2f pos);

    // Grabs a control point when the mouse cursor is over one. Returns true when a control
    // point is grabbed and false otherwise.
    bool selectControlPoint(Vector2f m);

    // Moves the 4 vertices by the given offset. Returns true when the moved vertices
    // are lying in the range [0,1] and false otherwise.
    bool move(Vector2f offset);

	// Load the parameters of the primitve
	void load(FILE *fp);

	// Save the parameters of the primitive
	void save(FILE *fp);

protected:
    // Paints the space between the both splines. steps_ triangles in a
    // trianglestrip are used for that.
    void paintInner();

protected:
    Vector2f coords[4]; ///< coordinates of the 4 vertices
    int steps;			///< number of triangles used to fill the space between the both splines
    bool scaleCoords;	///< indicates whether the coordinates must be scaled in paintInner() or not
};

#endif 