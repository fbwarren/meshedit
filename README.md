You can view the spec for this project at [MeshEdit](https://cs184.eecs.berkeley.edu/sp21/docs/proj2).

## **Contents** <!-- omit in toc -->

- [**Overview**](#overview)
- [**Section I: Bezier Curves and Surfaces**](#section-i-bezier-curves-and-surfaces)
  - [**Part 1: de Casteljau's Algorithm**](#part-1-de-casteljaus-algorithm)
  - [**Part 2: Bezier Surfaces with Separable 1D de Casteljau**](#part-2-bezier-surfaces-with-separable-1d-de-casteljau)
- [**Section II: Triangle Meshes and Half-Edge Data**](#section-ii-triangle-meshes-and-half-edge-data)
  - [**Part 3: Area-Weighted Vertex Normals**](#part-3-area-weighted-vertex-normals)
  - [**Part 4: Edge Flip**](#part-4-edge-flip)
  - [**Part 5: Edge Split**](#part-5-edge-split)
  - [**Part 6: Loop Subdivision for Mesh Upsampling**](#part-6-loop-subdivision-for-mesh-upsampling)

## **Overview**

The purpose of this project was to implement explore topics on geometric modeing, including Bezier curves and surfaces, the de Casteljau algorithm for calculating those, manipulating triangle meshes, and loop subdivision. The first section of this project deals with Bezier curves and surfaces, and the second section deals with triangle meshes and the half-edge data structure.  
I completed this project alone.

## **Section I: Bezier Curves and Surfaces**

[Bezier curves](https://en.wikipedia.org/wiki/B%C3%A9zier_curve) are the most popular way to create curves from a set of points. A Bezier Curve can be defined with two or more control points. $(n+1)$ points are needed to define a Bezier curve of degree $n$. There's multiple ways to define a Bezier curve given a set of points. One could use linear algebra to define a degree $n$ polynomial given $(n+1)$ points, and plot the polynomial to include the start and end points. There is a more practical method called [de Casteljau's algorithm](https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm).

### **Part 1: de Casteljau's Algorithm**  

[de Casteljau's algorithm](https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm) is a recursive algorithm for evaluating Bezier curves. Basically, it takes a set of points along with a parameter $t$ and performs linear interpolations on each pair of neighboring points. Then this same calculation is performed recursively on these intermediate points using the same parameter $t$ until no more pairs of intermediate points can be found. At the end of this algorithm, we are left with a single point that will be on the final Bezier curve.  

If we perform this algorithm for a large number of $t$ values, we will be able to create a smooth curve.  
Here is an animation that illustrates the relationship between $t$, the control points, and the final curve:  
<center><img src="./docs/images/bezier.gif" width="250"></center>  

Here's an example of what the recursive de Casteljau algorithm looks like at each step for some set of 6 control points and some $t$. In the last subdivision, the final point is shown in red. In the final image, I show the final result for a different $t$ value.  

| <img src="./docs/images/proj2_2.png" width=250>| <img src="./docs/images/proj2_3.png" width=250> | <img src="./docs/images/proj2_4.png" width=250> |
|:--:|:--:|:--:|
| Before | First subdivision | second subdivision |

| <img src="./docs/images/proj2_5.png" width=250>| <img src="./docs/images/proj2_6.png" width=250> | <img src="./docs/images/proj2_7.png" width=250> |
|:--:|:--:|:--:|
| Third subdivision | Fourth subdivision | Fifth (final) subdivision |

| <img src="./docs/images/proj2_8.png" width=250> |
|:--:|
| Different $t$ value |

### **Part 2: Bezier Surfaces with Separable 1D de Casteljau**

In this part, the de Casteljau algorithm will be extended to create curved surfaces in three dimensions. The implementation for this is pretty much the same as for Bezier curves, except now we have 4 rows of 4 points.Each row of four points represents a Bezier curve that is parameterized by some value $u$, and after the four Bezier curves are created, then we can create another Bezier curve along the other axis by using the four points at $u$ on the four Bezier curves, and parameterizing them on $v$.  

Here's the [Utah teapot](https://en.wikipedia.org/wiki/Utah_teapot) rendered using Bezier surfaces:  
<center><img src="./docs/images/proj2_9.png" width=600></center>

## **Section II: Triangle Meshes and Half-Edge Data**

In this section, we will use triangle meshes to model 3D objects. Although Bezier surfaces are better at representing curves smoothly and require less memory than triangle meshes, triangle meshes are used more frequently because Bezier surfaces because it's difficult to calculate their intersections with other surfaces.  

A convenient data structure for triangle meshes is the Half-Edge data structure. The main idea for this data structure is that each face in the mesh is composed of half edges such that the ordering of the each face is consistent. To enable convnient iteration of vertices, edges, and faces, each half-edge has useful members. Also, there are Vertex, Edge, and Face objects which have their own useful functions.

```C++
struct Halfedge {
  Halfedge *twin;   // the Halfedge opposite to this Halfedge
  Halfedge *next;   // the next Halfedge on the same face as this Halfedge
  Vertex *vertex;   // the tail of this Halfedge
  Edge *edge;       // the undirected edge on the mesh
  Face *face;       // the face to which this Halfedge belongs
}
```

### **Part 3: Area-Weighted Vertex Normals**

A simple method for shading known as [flat shading](https://en.wikipedia.org/wiki/Shading#Flat_shading) is already implemented at this point in the project. In this shading technique, lighting is evaluated once per face, where the intensity of the light to be displayed on a face is determined based on the normal to that face. (i.e. how far away the face is from the light, and the orientation of the face in relation to the light). Because the light on each face is only evaluated once per face, the result is not very smooth.  

To achieve better shading, we can interpolate the surface normals of the faces. This is done by calculating, for each vertex, a normalized vector derived from the weighted sum of the surface normals of each face it belongs to. A do-while loop can be used to iterate through the faces associated with some vertex `v`:  

```C++
Halfedge CIter h = v->halfedge();
do {
  // Process h->face() in this loop
  h = h->twin()->next();
} while (h != v->halfedge());
```

Here's what the Utah teapot looks with flat shading, and with Phong shading:

| <img src="./docs/images/proj2_10.png" width=600> | <img src="./docs/images/proj2_11.png" width=600> |
|:--:|:--:|
| Flat Shading | Phong Shading |

### **Part 4: Edge Flip**

One of the basic operations that a half-edge data structure needs to provide is edge flips. Given a pair of triangles, a flip operation performed on their shared edge will convert the original pair of triangles into a new pair that share an edge formed by the vertices that were *not* in the original shared edge:  

<center><img src="./docs/images/flip.jpg" width=500></center>  

Implementing this means operation changing the pointers for each half edge object relating to the two triangles. The process itself isn't too bad, especially because the process is fully spelled out [here](http://15462.courses.cs.cmu.edu/fall2015content/misc/HalfedgeEdgeOpImplementationGuide.pdf). The process is further simplified since halfedge objects have a member function that can set all of its pointers at once:

```C++
void setNeighbors(HalfedgeIter next, HalfedgeIter twin, VertexIter vertex, EdgeIter edge, FaceIter face)
```

Here's a mesh of a cute cow before and after some edge flips:

| <img src="./docs/images/proj2_13.png" width=600> | <img src="./docs/images/proj2_14.png" width=600> |
|:--:|:--:|
| Before flips | After flips |

### **Part 5: Edge Split**

Another basic operation for half-edge data structures is edge splits. Given a pair of triangles, a split operation on their shared edge creates a new vertex in the center of the edge and then edges are formed between the new vertex and every other vertex in the two triangles:  

<center><img src="./docs/images/split.jpg" width=500></center>  

The implementation for this is similar to the implementation for edge flips. Basically, I drew two diagrams of set of triangles with a shared edge, before and after a split operation on the shared edge. The diagrams had all of the vertices, faces, edges, and half edges labelled. Writing the split function was just a matter of carefully representing the "after" diagram in terms of the half-edge object and pointers.  

Here's a cube before and after some flip and split operations:

| <img src="./docs/images/proj2_16.png" width=600> | <img src="./docs/images/proj2_17.png" width=600> |
|:--:|:--:|
| Before | After |

### **Part 6: Loop Subdivision for Mesh Upsampling**

To achieve very smooth, high definition surfaces requires a large amount of vertices in a mesh. Specifying these points manually in a file will naturally take more effort and memory. Just like with antialiasing, there are useful techniques to convert from a coarse mesh to a higher resolution one. Unfortunately, the methods used to antialias a set of pixel values does not translate to 3D meshes. One reason for this is that pixels are in discrete locations while the vertices in a mesh are real valued!  

The simplest way to upsample a triangle mesh is a method called [loop subdivision](https://en.wikipedia.org/wiki/Loop_subdivision_surface). The basic idea for loop subdivision is to divide each triangle in a mesh into four smaller triangles, and update the location of each of the vertices based on some weighting rule. In this project, I implemented loop subdivision upsampling by:

1. Calculate all of the new positions (as a weighted sum of neighbors) for the already existing vertices and caching those new positions
2. Calculate and cache the positions of the vertices that will be created as a result of split operations that occur in the next step.
3. Split all of the *original* edges and assign the newly created vertices their positions from the values cached in the previous step. It's important that edges and vertices are being assigned a flag `isnew` as appropriate to avoid splitting edges infinitely.
4. Flip all edges that are between an old and new vertex.
5. Update vertex positions from cache.

I faced three annoying bugs when implementing loop subdivision:  

1. **Integer division**: The expression I had originally used to weight vertices was in the form of integer division, and *not* floating point division like I had intended. Because weights are a real value between 0 and 1, integer division will always result in a 0 (integer division takes the floor of the result). This led to every face being destroyed after a single loop subdivision step.
2. **Tracking old edges**: I struggled with figuring out how to keep track of and split only the original edges while skipping edges that were being created as the old ones were being split. I eventually just cached these edges in a vector while processing the edges in a previous step (thanks piazza).
3. **Flipping the desired edges**: This bug took me the longest by far. I think I spent about half my time on this project trying to figure this one out. When I implemented method for splitting edges, I followed the spec to the T, including the part asserts that the halfedge of a new vertex should point along the edge that was split. I had forgotten this, and when I split every edge in the mesh, I was setting `isNew` values for the four resulting edges to the wrong value! Everything else was fine in my implementation, but this little mistake made the results of loop subdivision look straight up violent.  

Here's an example of using loop subdivision to make a velvety chocolate donut:

| <img src="./docs/images/proj2_18.png" width=300> | <img src="./docs/images/proj2_19.png" width=300> | <img src="./docs/images/proj2_21.png" width=300> | <img src="./docs/images/proj2_22.png" width=300> |
|:--:|:--:|:--:|:--:|
| 0 subdivisions | 1 subdivisions | 2 subdivisions | 5 subdivisions |

Notice how all of the edges become smoother. We can preserve edges by pre-splitting them. Because splitting an edge creates a new vertex at that edge, pre-splitting an edge multiple times creates the effect of having a higher density of vertices along that edge. This will subdue the effect of loop subdivision by giving the edge a total higher weight. Here's an example with the doughnut again:

| <img src="./docs/images/proj2_24.png" width=300> | <img src="./docs/images/proj2_29.png" width=300> | <img src="./docs/images/proj2_30.png" width=300> | <img src="./docs/images/proj2_31.png" width=300> |
|:--:|:--:|:--:|:--:|
| Original surface | Pre-split edges | After 1 subdivision | After 5 subdivisions |

Using this same idea, we can help a very low resolution cube transform into higher resolution while minimising the loss of its defining features. Basically, all I did was split edges evenly across the faces so the net effect was similar to having the same cube but defined with more vertices. The more vertices you start out with before loop subdivision, the more you can retain sharp features like edges and corners.

| <img src="./docs/images/proj2_32.png" width=300> | <img src="./docs/images/proj2_40.png" width=300> | <img src="./docs/images/proj2_41.png" width=300> | <img src="./docs/images/proj2_42.png" width=300> |
|:--:|:--:|:--:|:--:|
| Original cube | Pre-split edges | After 1 subdivision | After 3 subdivisions |
