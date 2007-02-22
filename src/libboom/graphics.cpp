/*
	Graphics implementation
	
	Copyright 2006 Daniel G. Taylor
*/

#include <algorithm>
#include <cmath>

#include "graphics.h"
#include "stringfuncs.h"
#include "log.h"
#include "loadpng.h"
#include "vfs.h"

using namespace std;

//#define DISPLAY_MESH_HULLS = 1

namespace Boom
{
	namespace Graphics
	{
		//----------------------------------------------------------------------
		Point2d::Point2d(float x, float y)
		{
			this->x = x;
			this->y = y;
		}
		
		//----------------------------------------------------------------------
		Point3d::Point3d(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}
		
		//----------------------------------------------------------------------
		void Material::set()
		{	
			glMaterialfv(GL_FRONT, GL_AMBIENT, &(ambient.red));
			glMaterialfv(GL_FRONT, GL_DIFFUSE, &(diffuse.red));
			glMaterialfv(GL_FRONT, GL_SPECULAR, &(specular.red));
			glMateriali(GL_FRONT, GL_SHININESS, shininess);
			
			if (textured)
			{
				glBindTexture(GL_TEXTURE_2D, texture);
			}
		}
		
		//----------------------------------------------------------------------
		void Mesh::clear()
		{
			vertices.clear();
			normals.clear();
			texture_coords.clear();
			polygons.clear();
			materials.clear();
			textured = false;
		}
		
		//----------------------------------------------------------------------
		void Mesh::load_materials(const char *filename)
		{
			VirtualFS::File *data;
			Material *current = NULL;
			string label;
			
			LOG_DEBUG << "Loading " << filename << endl;
			
			data = VirtualFS::open(filename);
			if (!data)
			{
				return;
			}
			
			do
			{
				*data >> label;
				if (label == "newmtl")
				{
					if (current != NULL)
					{
						materials[current->name] = *current;
						delete current;
					}
					current = new Material();
					*data >> label;
					current->name = label;
					current->ambient.red = current->ambient.green
										 = current->ambient.blue = 0;
					current->diffuse.red = current->diffuse.green
										 = current->diffuse.blue = 0;
					current->specular.red = current->specular.green
										  = current->specular.blue = 0;
					current->alpha = 1.0;
					current->shininess = 96;
					current->texture = 0;
					current->textured = false;
				}
				else if (label == "Ka")
				{
					*data >> current->ambient.red
						 >> current->ambient.green
						 >> current->ambient.blue;
				}
				else if (label == "Kd")
				{
					*data >> current->diffuse.red
						 >> current->diffuse.green
						 >> current->diffuse.blue;
				}
				else if (label == "Ks")
				{
					*data >> current->specular.red
						 >> current->specular.green
						 >> current->specular.blue;
				}
				else if (label == "d")
				{
					*data >> current->alpha;
				}
				else if (label == "Ns")
				{
					*data >> current->shininess;
				}
				else if (label == "map_Kd")
				{
					GLuint texture;
					PNGData image;
					
					*data >> label;
					label = "Images/" + label;
					
					//current->textured = false;
					
					load_png(label.c_str(), image);
					
					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);
					
					if (image.alpha)
					{
						current->texture_alpha = true;
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, 
							image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
					}
					else
					{
						current->texture_alpha = false;
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, 
							image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
					}		
					//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, info[0], 
					//		info[1], GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					current->texture = texture;
					current->textured = true;
					textured = true;
				}
			} while (*data);
			materials[current->name] = *current;
			delete current;
			
			data->close();
			delete data;
			
			LOG_DEBUG << "Loaded " << materials.size() << " materials." << endl;
		}
		
		//----------------------------------------------------------------------
		void Mesh::load(const char *filename)
		{
			VirtualFS::File *data;
			string label;
			float x, y, z;
			int i;
			Material *current_material = NULL;
			
			LOG_INFO << "Loading " << filename << endl;
			
			data = VirtualFS::open(filename);
			if (!data)
			{
				return;
			}
			
			clear();
			
			do
			{
				*data >> label;
				if (label == "mtllib")
				{
					*data >> label;
					label = "Meshes/" + label;
					load_materials(label.c_str());
				}
				else if (label == "vn")
				{
					Point3d normal;
					*data >> x >> y >> z;
					normal.x = x;
					normal.y = y;
					normal.z = z;
					
					normals.push_back(normal);
				}
				else if (label == "vt")
				{
					Point2d tex;
					*data >> x >> y;
					tex.x = x;
					tex.y = y;
					
					texture_coords.push_back(tex);
				}
				else if (label == "v")
				{
					Point3d vertex;
					*data >> x >> y >> z;
					vertex.x = x;
					vertex.y = y;
					vertex.z = z;
					
					vertices.push_back(vertex);
				}
				else if (label == "usemtl")
				{
					*data >> label;
					current_material = &materials[label];
				}
				else if (label == "f")
				{
					MeshPolygon poly;
					char line[256];
					vector <string> polys;
					
					data->getline(line, 256);
					
					split(line, polys, " ");
					
					if (polys.size() < 1)
						continue;
					
					for (unsigned int x = 0; x < polys.size(); x++)
					{
						MeshVertex vertex;
						vector <string> parts;
						
						split(polys[x], parts, "/");
						switch (parts.size())
						{
							case 1:
								i = atoi(parts[0].c_str()) - 1;
								vertex.pos = i;
								break;
							case 2:
								i = atoi(parts[0].c_str()) - 1;
								vertex.pos = i;
								i = atoi(parts[1].c_str()) - 1;
								vertex.normal = i;
								vertex.texture_coord = -1;
								break;
							case 3:
								i = atoi(parts[0].c_str()) - 1;
								vertex.pos = i;
								i = atoi(parts[1].c_str()) - 1;
								vertex.texture_coord = i;
								i = atoi(parts[2].c_str()) - 1;
								vertex.normal = i;
								break;
							default:
								LOG_ERROR << "Wrong size... (" << parts.size() 
										  << ") in " << polys[x] << endl;
						}
						poly.vertices.push_back(vertex);
					}
					poly.material = current_material;
					polygons.push_back(poly);
				}
			} while (*data);
			
			data->close();
			delete data;
			
			LOG_DEBUG << "Loaded " << normals.size() << " normals and " 
					 << texture_coords.size() << " texture coordinates." << endl;
			LOG_DEBUG << "Loaded " << vertices.size() << " vertices in "
					 << polygons.size() << " polygons." << endl;
			
			hull.generate(vertices);
			hull.calc_center();
			hull.calc_radius();
			hull.optimize(6);
			
			generate_display_list();
		}
		
		//----------------------------------------------------------------------
		// TODO: Use vertex lists???
		void Mesh::generate_display_list()
		{
			Material *last = NULL;
			
			display_list = glGenLists(1);
			glNewList(display_list, GL_COMPILE);
			for (unsigned int x = 0; x < polygons.size(); x++)
			{
				if (polygons[x].material != NULL)
				{
					if (last == NULL || polygons[x].material != last)
					{
						polygons[x].material->set();
						last = polygons[x].material;
					}
				}
				
				glBegin(GL_POLYGON);
				for (unsigned int y = 0; y < polygons[x].vertices.size(); y++)
				{
					if (polygons[x].vertices[y].texture_coord != -1)
					{
						glTexCoord2f(texture_coords[polygons[x].vertices[y].texture_coord].x,
									 texture_coords[polygons[x].vertices[y].texture_coord].y);
					}
					if (polygons[x].vertices[y].normal != -1)
					{
						glNormal3fv(&(normals[polygons[x].vertices[y].normal].x));
					}
					glVertex3fv(&(vertices[polygons[x].vertices[y].pos].x));
				}
				glEnd();
			}
			
			glEndList();
		}
		
		//----------------------------------------------------------------------
		void Mesh::render()
		{
			if (textured)
			{
				glCallList(display_list);
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				glCallList(display_list);
				glEnable(GL_TEXTURE_2D);
			}
			
			#ifdef DISPLAY_MESH_HULLS
				
				glDisable(GL_LIGHTING);
				glDisable(GL_TEXTURE_2D);
			
				glColor3f(0.0, 0.8, 0.0);
			
				glBegin(GL_LINE_LOOP);
				for (vector <Point2d>::iterator vertex = hull.vertices.begin();
					 vertex != hull.vertices.end(); vertex++)
				{
					glVertex3f(vertex->x, vertex->y, 0.1);
				}
				glEnd();
			
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_LIGHTING);
			
			#endif
		}
		
		//----------------------------------------------------------------------
		BMeshPoint::BMeshPoint()
		{
			vertex = NULL;
			normal = NULL;
			texture_coord = NULL;
		}
		
		//----------------------------------------------------------------------
		BMeshPoly::BMeshPoly(Material *material)
		{
			this->material = material;
		}
		
		//----------------------------------------------------------------------
		BMeshPoly::~BMeshPoly()
		{
			vector <BMeshPoint*>::iterator i;
			
			for (i = points.begin(); i != points.end(); i++)
			{
				delete *i;
			}
			
			points.clear();
		}
		
		//----------------------------------------------------------------------
		BMeshFrame::~BMeshFrame()
		{
			vector <BMeshPoly*>::iterator i;
			
			for (i = polys.begin(); i != polys.end(); i++)
			{
				delete *i;
			}
			
			polys.clear();
			
			glDeleteLists(display_list, 1);
		}
		
		//----------------------------------------------------------------------
		void BMeshFrame::generate_display_list()
		{
			Material *last = NULL;
			
			display_list = glGenLists(1);
			glNewList(display_list, GL_COMPILE);
			for (unsigned int x = 0; x < polys.size(); x++)
			{
				BMeshPoly *poly = polys[x];
				if (poly->material != NULL)
				{
					if (last == NULL || poly->material != last)
					{
						poly->material->set();
						last = poly->material;
					}
				}
				
				glBegin(GL_POLYGON);
				for (unsigned int y = 0; y < poly->points.size(); y++)
				{
					BMeshPoint *point = poly->points[y];
					if (point->texture_coord != NULL)
					{
						glTexCoord2f(point->texture_coord->x,
									 point->texture_coord->y);
					}
					if (point->normal != NULL)
					{
						glNormal3fv(&(point->normal->x));
					}
					glVertex3fv(&(point->vertex->x));
				}
				glEnd();
			}
			glEndList();
		}
		
		//----------------------------------------------------------------------
		BMeshAnimation::BMeshAnimation(string name)
		{
			this->name = name;
		}
		
		//----------------------------------------------------------------------
		BMeshAnimation::~BMeshAnimation()
		{
			vector <BMeshFrame*>::iterator i;
			
			for (i = frames.begin(); i != frames.end(); i++)
			{
				delete *i;
			}
			
			frames.clear();
		}
		
		//----------------------------------------------------------------------
		BMesh::~BMesh()
		{
			map <string, BMeshAnimation*>::iterator i;
			
			for (i = animations.begin(); i != animations.end(); i++)
			{
				delete i->second;
			}
			
			vertices.clear();
			normals.clear();
			texture_coords.clear();
			materials.clear();
			animations.clear();
		}
		
		//----------------------------------------------------------------------
		void BMesh::load_materials(const char *filename)
		{
			VirtualFS::File *data;
			Material *current = NULL;
			string label;
			
			LOG_DEBUG << "Loading " << filename << endl;
			
			data = VirtualFS::open(filename);
			if (!data)
			{
				return;
			}
			
			textured = false;
			
			do
			{
				*data >> label;
				if (label == "material")
				{
					if (current != NULL)
					{
						materials[current->name] = *current;
						delete current;
					}
					current = new Material();
					*data >> label;
					current->name = label;
					current->ambient.red = current->ambient.green
										 = current->ambient.blue = 0;
					current->diffuse.red = current->diffuse.green
										 = current->diffuse.blue = 0;
					current->specular.red = current->specular.green
										  = current->specular.blue = 0;
					current->alpha = 1.0;
					current->shininess = 96;
					current->texture = 0;
					current->textured = false;
				}
				else if (label == "ambient")
				{
					*data >> current->ambient.red
						 >> current->ambient.green
						 >> current->ambient.blue;
				}
				else if (label == "diffuse")
				{
					*data >> current->diffuse.red
						 >> current->diffuse.green
						 >> current->diffuse.blue;
				}
				else if (label == "specular")
				{
					*data >> current->specular.red
						 >> current->specular.green
						 >> current->specular.blue;
				}
				else if (label == "alpha")
				{
					*data >> current->alpha;
				}
				else if (label == "shiny")
				{
					*data >> current->shininess;
				}
				else if (label == "texture")
				{
					GLuint texture;
					PNGData image;
					
					*data >> label;
					label = "Images/" + label;
					
					//current->textured = false;
					
					load_png(label.c_str(), image);
					
					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);
					
					if (image.alpha)
					{
						current->texture_alpha = true;
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, 
							image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
					}
					else
					{
						current->texture_alpha = false;
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, 
							image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
					}		
					//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, info[0], 
					//		info[1], GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					current->texture = texture;
					current->textured = true;
					textured = true;
				}
			} while (*data);
			materials[current->name] = *current;
			delete current;
			
			data->close();
			delete data;
			
			LOG_DEBUG << "Loaded " << materials.size() << " materials." << endl;
		}
		
		//----------------------------------------------------------------------
		void BMesh::load(const char *filename)
		{
			VirtualFS::File *data;
			string label;
			float x, y, z;
			int i;
			
			int anim_count = 0, frame_count = 0, poly_count = 0;
			
			Material *current_material = NULL;
			
			BMeshAnimation *anim = NULL;
			BMeshFrame *frame = NULL;
			BMeshPoly *poly = NULL;
			BMeshPoint *point = NULL;
			
			LOG_INFO << "Loading " << filename << endl;
			
			data = VirtualFS::open(filename);
			if (!data)
			{
				LOG_ERROR << "File open error..." << endl;
				return;
			}
			
			//clear();
			
			do
			{
				*data >> label;
				if (!(*data)) break;
				//LOG_INFO << label << endl;
				if (label == "materials")		// load material library
				{
					*data >> label;
					label = "Meshes/" + label;
					load_materials(label.c_str());
				}
				else if (label == "v")			// vertex
				{
					Point3d vertex;
					
					*data >> vertex.x >> vertex.y >> vertex.z;
					
					vertices.push_back(vertex);
				}
				else if (label == "vn")			// vertex normal
				{
					Point3d normal;
					
					*data >> normal.x >> normal.y >> normal.z;
					
					normals.push_back(normal);
				}
				else if (label == "vt")			// vertex texture coordinate
				{
					Point2d texture_coord;
					
					*data >> texture_coord.x >> texture_coord.y;
					
					texture_coord.y = -texture_coord.y;
					
					texture_coords.push_back(texture_coord);
				}
				else if (label == "animation")	// start a new animation
				{
					if (anim)
					{
						if (frame)
						{
							anim->frames.push_back(frame);
							frame = NULL;
						}
						animations[anim->name] = anim;
					}
					*data >> label;
					anim = new BMeshAnimation(label);
					anim_count++;
				}
				else if (label == "speed")		// set animation speed
				{
					*data >> anim->speed;
				}
				else if (label == "frame")		// start a new frame
				{
					if (frame)
					{
						if (poly)
						{
							frame->polys.push_back(poly);
							poly = NULL;
						}
						anim->frames.push_back(frame);
					}
					frame = new BMeshFrame();
					frame_count++;
				}
				else if (label == "poly")		// start a new polygon
				{
					if (poly)
					{
						if (point)
						{
							poly->points.push_back(point);
							point = NULL;
						}
						frame->polys.push_back(poly);
					}
					*data >> label;
					poly = new BMeshPoly(&materials[label]);
					poly_count++;
				}
				else if (label == "point")		// add a point to the polygon
				{
					if (point)
					{
						poly->points.push_back(point);
					}
					point = new BMeshPoint();
					*data >> i;
					point->vertex = &vertices[i];
					//LOG_INFO << i << ", " << point->vertex->x << ", " << point->vertex->y << ", " << point->vertex->z << endl;
					*data >> i;
					point->normal = &normals[i];
					*data >> i;
					point->texture_coord = &texture_coords[i];
				}
				else
				{
					LOG_WARNING << "Unknown identifier '" << label << "'" << endl;
				}
			} while (*data);
			
			if (point)
				poly->points.push_back(point);
			if (poly)
				frame->polys.push_back(poly);
			if (frame)
				anim->frames.push_back(frame);
			if (anim)
				animations[anim->name] = anim;
			
			data->close();
			delete data;
			
			LOG_DEBUG << "Loaded " << vertices.size() << " vertices" << endl;
			LOG_DEBUG << "Loaded " << normals.size() << " normals" << endl;
			LOG_DEBUG << "Loaded " << texture_coords.size() << " texture coordinates" << endl;
			LOG_DEBUG << "Loaded " << anim_count << " animation(s)" << endl;
			LOG_DEBUG << "Loaded " << frame_count << " frame(s)" << endl;
			LOG_DEBUG << "Loaded " << poly_count << " polygon(s)" << endl;
		
			LOG_DEBUG << "Generating display lists..." << endl;
			generate_display_lists();
		}
		
		//----------------------------------------------------------------------
		void BMesh::generate_display_lists()
		{
			map <string, BMeshAnimation*>::iterator i;
			vector <BMeshFrame*>::iterator j;
			
			for (i = animations.begin(); i != animations.end(); i++)
			{
				//LOG_DEBUG << "Generating display list for " << i->second->name << endl;
				for (j = i->second->frames.begin(); j != i->second->frames.end(); j++)
				{
					(*j)->generate_display_list();
				}
			}
		}
		
		//----------------------------------------------------------------------
		void BMesh::render(string animation, int frame)
		{
			if (textured)
			{
				glCallList(animations[animation]->frames[frame]->display_list);
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				glCallList(animations[animation]->frames[frame]->display_list);
				glEnable(GL_TEXTURE_2D);
			}
		}
		
		//----------------------------------------------------------------------
		Point3d BMesh::get_size()
		{
			Point3d low, high;
			vector <Graphics::Point3d>::iterator i;
			
			for (i = vertices.begin(); i != vertices.end(); i++)
			{
				if (i->x < low.x)  low.x = i->x;
				if (i->y < low.y)  low.y = i->y;
				if (i->z < low.z)  low.z = i->z;
				if (i->x > high.x) high.x = i->x;
				if (i->y > high.y) high.y = i->y;
				if (i->z > high.z) high.z = i->z;
			}
			
			return Point3d(high.x - low.x, high.y - low.y, high.z - low.z);
		}
		
		//----------------------------------------------------------------------
		bool HullVertex2d_cmp(HullVertex2d v1, HullVertex2d v2)
		{
			if (v1.angle < v2.angle)
				return true;
			else if (v1.angle > v2.angle)
				return false;
			else if (v1.x == v2.x && v1.y == v2.y)
				return false;
			else if (distance2d(v1.pivot, &v1) < distance2d(v1.pivot, &v2))
				return true;
			else
				return false;
		}
		
		//----------------------------------------------------------------------
		Hull2d::Hull2d()
		{
			radius = 0;
		}
		
		//----------------------------------------------------------------------
		void Hull2d::generate(vector <Point3d> &mesh_verts)
		{
			vector <HullVertex2d> vdata;
			HullVertex2d vtemp;
			Point2d *pivot;
			Point3d *pivot_ptr;
			unsigned int size;
			
			// Create a list of pointers to the vertex data that we can sort
			// and find the bottom-left most vertex.
			pivot_ptr = &(* mesh_verts.begin());
			for(vector <Point3d>::iterator vertex = mesh_verts.begin();
				vertex != mesh_verts.end(); vertex++)
			{
				if (vertex->y < pivot_ptr->y)
				{
					pivot_ptr = &(*vertex);
				}
				else if (vertex->y == pivot_ptr->y && vertex->x < pivot_ptr->x)
				{
					pivot_ptr = &(*vertex);
				}
				vtemp.x = vertex->x;
				vtemp.y = vertex->y;
				vtemp.angle = 0;
				vdata.push_back(vtemp);
			}
			pivot = new Point2d();
			pivot->x = pivot_ptr->x;
			pivot->y = pivot_ptr->y;
			
			// Create a list of angles corresponding to each point from the pivot
			for (vector <HullVertex2d>::iterator vertex = vdata.begin();
				 vertex != vdata.end(); vertex++)
			{
				vertex->pivot = pivot;
				vertex->angle = polar_angle2d(pivot, &(*vertex));
			}
			
			sort (vdata.begin(), vdata.end(), HullVertex2d_cmp);
			
			vertices.clear();
			vertices.push_back(vdata[0]);
			vertices.push_back(vdata[1]);
			
			size = vdata.size();
			for (unsigned int x = 2; x < size; x++)
			{	
				double c = cross2d(&vertices[vertices.size() - 2],
								   &vertices[vertices.size() - 1], &vdata[x]);
				
				if (c == 0)
				{
					vertices.pop_back();
					vertices.push_back(vdata[x]);
				}
				else if (c > 0)
				{
					vertices.push_back(vdata[x]);
				}
				else
				{
					while (c <= 0 && vertices.size() > 2)
					{
						vertices.pop_back();
						c = cross2d(&vertices[vertices.size() - 2],
									&vertices[vertices.size() - 1], &vdata[x]);
					}
					vertices.push_back(vdata[x]);
				}
			}
			
			LOG_DEBUG << "Hull generated with " << vertices.size()
					  << " points." << endl;
			
			delete pivot;
		}
		
		//----------------------------------------------------------------------
		void Hull2d::calc_center()
		{
			vector <Point2d>::iterator vertex;
			
			center.x = center.y = 0;
			for(vertex = vertices.begin(); vertex != vertices.end(); vertex++)
			{
				center.x += vertex->x;
				center.y += vertex->y;
			}
			
			center.x /= float(vertices.size());
			center.y /= float(vertices.size());
		}
		
		//----------------------------------------------------------------------
		void Hull2d::calc_radius()
		{
			float max_dist = 0;
			float dx, dy, dist_squared;
			vector <Point2d>::iterator vertex;
			
			for (vertex = vertices.begin(); vertex != vertices.end(); vertex++)
			{
				dx = center.x - vertex->x;
				dy = center.y - vertex->y;
				dist_squared = (dx * dx) + (dy * dy);
				if (dist_squared > max_dist)
				{
					max_dist = dist_squared;
				}
			}
			
			radius = max_dist;
		}
		
		//----------------------------------------------------------------------
		void Hull2d::optimize(unsigned int size)
		{
			if (vertices.size() <= size)
			{
				return;
			}
			
			double step = 2 * M_PI / double(size);
			double current_angle = 0;
			vector <Point2d> new_hull;
			
			for (unsigned int x = 0; x < size; x++)
			{
				Point2d n;
				double min_d = 100000;
				Point2d *min_p;
				
				n.x = center.x + (radius * cos(current_angle));
				n.y = center.y + (radius * sin(current_angle));
				
				for (vector <Point2d>::iterator vertex = vertices.begin();
					 vertex != vertices.end(); vertex++)
				{
					double d = distance2d(&(*vertex), &n);
					if (d < min_d)
					{
						min_d = d;
						min_p = &(*vertex);
					}
				}
				
				new_hull.push_back(*min_p);
				current_angle += step;
			}
			
			vertices.clear();
			vertices = new_hull;
			
			LOG_DEBUG << "Hull optimized to " << vertices.size()
					  << " points." << endl;
		}
		
		//----------------------------------------------------------------------
		double distance2d(Point2d *v1, Point2d *v2)
		{
			return (v1->x - v2->x) * (v1->x - v2->x) +
				   (v1->y - v2->y) * (v1->y - v2->y);
		}
		
		//----------------------------------------------------------------------
		double distance2d(float x1, float y1, float x2, float y2)
		{
			return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
		}
		
		//----------------------------------------------------------------------
		double polar_angle2d(Point2d *pole, Point2d *point)
		{
			double dx, dy, angle;
			
			dx = double(point->x) - pole->x;
			dy = double(point->y) - pole->y;
			
			angle = atan2(dy, dx) + M_PI;
						
			return angle;
		}
		
		//----------------------------------------------------------------------
		double cross2d(Point2d *v1, Point2d *v2, Point2d *v3)
		{
			return ((v2->x - v1->x) * (v3->y - v1->y)) -
				   ((v3->x - v1->x) * (v2->y - v1->y));
		}
	}
}
