/*
	Graphics implementation
	
	Copyright 2006 Daniel G. Taylor
*/

#define DISPLAY_MESH_HULLS = 1;

#include <algorithm>
#include <cmath>

#include "graphics.h"
#include "stringfuncs.h"
#include "log.h"
#include "loadpng.h"
#include "vfs.h"

using namespace std;

namespace Boom
{
	namespace Graphics
	{
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
			
			#ifdef DISPLAY_MESH_HULLS
				
				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);
				glColor3f(0.0, 1.0, 0.0);
				
				glBegin(GL_LINE_LOOP);
				for (vector <Point2d>::iterator vertex = hull.vertices.begin();
					 vertex != hull.vertices.end(); vertex++)
				{
					glVertex3f(vertex->x, vertex->y, 0.1);
				}
				glEnd();
				
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_LIGHTING);
				
			#endif
			
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
		void Scene::clear()
		{
			objects.clear();
			meshes.clear();
		}
		
		//----------------------------------------------------------------------
		int Scene::add(const char *mesh_name, float x, float y, float z)
		{
			SceneObject obj;
			
			map <string, Mesh>::iterator pos = meshes.find(mesh_name);
			if (pos == meshes.end())
			{
				Mesh m;
				string filename;
				
				filename = "Meshes/" + string(mesh_name) + ".obj";
				m.load(filename.c_str());
				
				meshes[mesh_name] = m;
			}
			
			obj.mesh = mesh_name;
			obj.x = x;
			obj.y = y;
			obj.z = z;
			obj.render = true;
			
			objects[++current_id] = obj;
			
			return current_id;
		}
		
		//----------------------------------------------------------------------
		bool Scene::move(int id, float dx, float dy, float dz)
		{
			objects[id].x += dx;
			objects[id].y += dy;
			objects[id].z += dz;
			
			return false;
		}
		
		//----------------------------------------------------------------------
		void Scene::remove(int id)
		{
			objects.erase(id);
		}
		
		//----------------------------------------------------------------------
		void Scene::render()
		{
			SceneObject *current;
			
			for (map <unsigned int, SceneObject>::iterator pos = objects.begin();
													pos != objects.end(); pos++)
			{
				current = &(pos->second);
				if (current->render)
				{
					glPushMatrix();
					glTranslatef(current->x, current->y, current->z);
					meshes[current->mesh].render();
					glPopMatrix();
				}
			}
		}
		
		//----------------------------------------------------------------------
		double distance2d(Point2d *v1, Point2d *v2)
		{
			return (v1->x - v2->x) * (v1->x - v2->x) +
				   (v1->y - v2->y) * (v1->y - v2->y);
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
