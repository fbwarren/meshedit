#include "student_code.h"
#include "mutablePriorityQueue.h"

using namespace std;

namespace CGL
{

  /**
   * Evaluates one step of the de Casteljau's algorithm using the given points and
   * the scalar parameter t (class member).
   *
   * @param points A vector of points in 2D
   * @return A vector containing intermediate points or the final interpolated vector
   */
  std::vector<Vector2D> BezierCurve::evaluateStep(std::vector<Vector2D> const &points)
  {
    // TODO Part 1.
    vector<Vector2D> intermediates;
    for (int i=0; i < points.size()-1; i++) {
        intermediates.push_back(lerp(points[i], points[i+1], t));
    }
    return intermediates;
  }

  /**
   * Evaluates one step of the de Casteljau's algorithm using the given points and
   * the scalar parameter t (function parameter).
   *
   * @param points    A vector of points in 3D
   * @param t         Scalar interpolation parameter
   * @return A vector containing intermediate points or the final interpolated vector
   */
  std::vector<Vector3D> BezierPatch::evaluateStep(std::vector<Vector3D> const &points, double t) const
  {
    // TODO Part 2.
      vector<Vector3D> intermediates;
      for (int i=0; i < points.size()-1; i++) {
          intermediates.push_back(lerp(points[i], points[i+1], t));
      }
      return intermediates;
  }

  /**
   * Fully evaluates de Casteljau's algorithm for a vector of points at scalar parameter t
   *
   * @param points    A vector of points in 3D
   * @param t         Scalar interpolation parameter
   * @return Final interpolated vector
   */
  Vector3D BezierPatch::evaluate1D(std::vector<Vector3D> const &points, double t) const
  {
    // TODO Part 2.
    if (points.size() == 1)
        { return points[0]; }

    vector<Vector3D> curve;
    for (int i = 0; i < points.size()-1; i++) {
        curve.push_back(lerp(points[i], points[i+1], t));
    }
    return evaluate1D(curve, t);
  }

  /**
   * Evaluates the Bezier patch at parameter (u, v)
   *
   * @param u         Scalar interpolation parameter
   * @param v         Scalar interpolation parameter (along the other axis)
   * @return Final interpolated vector
   */
  Vector3D BezierPatch::evaluate(double u, double v) const
  {
    // TODO Part 2.
    vector<Vector3D> curves;
    for (int i=0; i < controlPoints.size(); i++)
        { curves.push_back(evaluate1D(controlPoints[i], u)); }
    return evaluate1D(curves, v);
  }

  Vector3D Vertex::normal( void ) const
  {
    // TODO Part 3.
    // Returns an approximate unit normal at this vertex, computed by
    // taking the area-weighted average of the normals of neighboring
    // triangles, then normalizing.
    Vector3D N(0., 0., 0.);
    HalfedgeCIter h = this->halfedge();
    do {
        N += (h->face())->normal();
        h = (h->twin())->next();
    } while (h != this->halfedge());
    return N.unit();
  }

  EdgeIter HalfedgeMesh::flipEdge( EdgeIter e0 )
  {
    // TODO Part 4.
    // This method should flip the given edge and return an iterator to the flipped edge.
    // Avoid flipping boundary edges
    if (e0->isBoundary())
        { return e0; }

    // Define halfedges
    HalfedgeIter h0 = e0->halfedge();
    HalfedgeIter h1 = h0->next();
    HalfedgeIter h2 = h1->next();
    HalfedgeIter h3 = h0->twin();
    HalfedgeIter h4 = h3->next();
    HalfedgeIter h5 = h4->next();
    HalfedgeIter h6 = h1->twin();
    HalfedgeIter h7 = h2->twin();
    HalfedgeIter h8 = h4->twin();
    HalfedgeIter h9 = h5->twin();

    // Define faces
    FaceIter f0 = h0->face();
    FaceIter f1 = h3->face();
    if (f0->isBoundary() || f1->isBoundary())
        { return e0; }

    // Define vertices
    VertexIter v0 = h0->vertex();
    VertexIter v1 = h3->vertex();
    VertexIter v2 = h6->vertex();
    VertexIter v3 = h5->vertex();

    // Define edges
    EdgeIter e1 = h1->edge();
    EdgeIter e2 = h2->edge();
    EdgeIter e3 = h4->edge();
    EdgeIter e4 = h5 -> edge();

    // Set each next, twin, vertex, edge, and face for each inner halfedge
    h0->setNeighbors(h1, h3, v3, e0, f0);
    h1->setNeighbors(h2, h7, v2, e2, f0);
    h2->setNeighbors(h0, h8, v0, e3, f0);
    h3->setNeighbors(h4, h0, v2, e0, f1);
    h4->setNeighbors(h5, h9, v3, e4, f1);
    h5->setNeighbors(h3, h6, v1, e1, f1);
    // Set values for outer halfedges
    h6->setNeighbors(h6->next(), h5, v2, e1, h6->face());
    h7->setNeighbors(h7->next(), h1, v0, e2, h7->face());
    h8->setNeighbors(h8->next(), h2, v3, e3, h8->face());
    h9->setNeighbors(h9->next(), h4, v1, e4, h9->face());

    // Set vertices
    v0->halfedge() = h2;
    v1->halfedge() = h5;
    v2->halfedge() = h3;
    v3->halfedge() = h0;

    // Edges:
    e0->halfedge() = h0;
    e1->halfedge() = h5;
    e2->halfedge() = h1;
    e3->halfedge() = h2;
    e4->halfedge() = h4;

    // Faces:
    f0->halfedge() = h0;
    f1->halfedge() = h3;

    return e0;
  }

  VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
  {
    // TODO Part 5.
    // This method should split the given edge and return an iterator to the newly inserted vertex.
    // The halfedge of this vertex should point along the edge that was split, rather than the new edges.
      if (e0->isBoundary())
      { return VertexIter(); }

      // Define halfedges
      HalfedgeIter h0 = e0->halfedge();
      HalfedgeIter h1 = h0->next();
      HalfedgeIter h2 = h1->next();
      HalfedgeIter h3 = h0->twin();
      HalfedgeIter h4 = h3->next();
      HalfedgeIter h5 = h4->next();
      HalfedgeIter h6 = h1->twin();
      HalfedgeIter h7 = h2->twin();
      HalfedgeIter h8 = h4->twin();
      HalfedgeIter h9 = h5->twin();
      HalfedgeIter h10 = newHalfedge();
      HalfedgeIter h11 = newHalfedge();
      HalfedgeIter h12 = newHalfedge();
      HalfedgeIter h13 = newHalfedge();
      HalfedgeIter h14 = newHalfedge();
      HalfedgeIter h15 = newHalfedge();

      // Define faces
      FaceIter f0 = h0->face();
      FaceIter f1 = h3->face();
      FaceIter f2 = newFace();
      FaceIter f3 = newFace();
      if (f0->isBoundary() || f1->isBoundary())
      { return VertexIter(); }

      // Define vertices
      VertexIter v0 = h0->vertex();
      VertexIter v1 = h3->vertex();
      VertexIter v2 = h6->vertex();
      VertexIter v3 = h5->vertex();
      VertexIter v4 = newVertex();

      // Define edges
      EdgeIter e1 = h1->edge();
      EdgeIter e2 = h2->edge();
      EdgeIter e3 = h4->edge();
      EdgeIter e4 = h5->edge();
      EdgeIter e5 = newEdge();
      EdgeIter e6 = newEdge();
      EdgeIter e7 = newEdge();

      // Set each next, twin, vertex, edge, and face for each halfedge
      h0->setNeighbors(h1, h3, v0, e6, f0);
      h1->setNeighbors(h2, h15, v4, e5, f0);
      h2->setNeighbors(h0, h7, v2, e2, f0);

      h3->setNeighbors(h4, h0, v4, e6, f1);
      h4->setNeighbors(h5, h8, v0, e3, f1);
      h5->setNeighbors(h3, h10, v3, e7, f1);

      // Set values for outer halfedges
      h6->setNeighbors(h6->next(), h14, v2, e1, h6->face());
      h7->setNeighbors(h7->next(), h2, v0, e2, h7->face());
      h8->setNeighbors(h8->next(), h4, v3, e3, h8->face());
      h9->setNeighbors(h9->next(), h11, v1, e4, h9->face());

      h10->setNeighbors(h11, h5, v4, e7, f2);
      h11->setNeighbors(h12, h9, v3, e4, f2);
      h12->setNeighbors(h10, h13, v1, e0, f2);

      h13->setNeighbors(h14, h12, v4, e0, f3);
      h14->setNeighbors(h15, h6, v1, e1, f3);
      h15->setNeighbors(h13, h1, v2, e5, f3);

      // Set vertices
      v0->halfedge() = h4;
      v1->halfedge() = h14;
      v2->halfedge() = h2;
      v3->halfedge() = h11;
      v4->halfedge() = h13;
      v4->position = lerp(v0->position, v1->position, 0.5);

      // Edges:
      e0->halfedge() = h13;
      e1->halfedge() = h14;
      e2->halfedge() = h2;
      e3->halfedge() = h4;
      e4->halfedge() = h11;
      e5->halfedge() = h1;
      e6->halfedge() = h0;
      e7->halfedge() = h5;

      // Faces:
      f0->halfedge() = h0;
      f1->halfedge() = h3;
      f2->halfedge() = h10;
      f3->halfedge() = h13;

      return v4;
  }

  Vector2D lerp(Vector2D p0, Vector2D p1, float t) {
      return ((1-t) * p0) + (t*p1);
  }

  Vector3D lerp(Vector3D p0, Vector3D p1, float t) {
      return ((1-t) * p0) + (t*p1);
  }

  void MeshResampler::upsample( HalfedgeMesh& mesh )
  {
    // TODO Part 6.
    // This routine should increase the number of triangles in the mesh using Loop subdivision.
    // One possible solution is to break up the method as listed below.

    // 1. Compute new positions for all the vertices in the input mesh, using the Loop subdivision rule,
    // and store them in Vertex::newPosition. At this point, we also want to mark each vertex as being
    // a vertex of the original mesh.

    // 2. Compute the updated vertex positions associated with edges, and store it in Edge::newPosition.

    // 3. Split every edge in the mesh, in any order. For future reference, we're also going to store some
    // information about which subdivide edges come from splitting an edge in the original mesh, and which edges
    // are new, by setting the flat Edge::isNew. Note that in this loop, we only want to iterate over edges of
    // the original mesh---otherwise, we'll end up splitting edges that we just split (and the loop will never end!)

    // 4. Flip any new edge that connects an old and new vertex.

    // 5. Copy the new vertex positions into final Vertex::position.

  }
}