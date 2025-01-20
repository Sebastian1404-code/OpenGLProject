#include "Mesh.hpp"
namespace gps {

	/* Mesh Constructor */
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {

		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->setupMesh();
	}

	Buffers Mesh::getBuffers() {
	    return this->buffers;
	}

	/* Mesh drawing function - also applies associated textures */
	void Mesh::Draw(gps::Shader shader)	{

		shader.useShaderProgram();

		//set textures
		for (GLuint i = 0; i < textures.size(); i++) {

			glActiveTexture(GL_TEXTURE0 + i);
			glUniform1i(glGetUniformLocation(shader.shaderProgram, this->textures[i].type.c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}

		glBindVertexArray(this->buffers.VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

        for(GLuint i = 0; i < this->textures.size(); i++) {

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    }

	// Initializes all the buffer objects/arrays
	void Mesh::setupMesh() {

		// Create buffers/arrays
		glGenVertexArrays(1, &this->buffers.VAO);
		glGenBuffers(1, &this->buffers.VBO);
		glGenBuffers(1, &this->buffers.EBO);

		glBindVertexArray(this->buffers.VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->buffers.VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}

	//raycasting
	/*void Mesh::BuildBVH() {
		bvhNodes.clear();
		int currentIndex = 0;
		BuildBVHRecursive(0, indices.size() / 3, currentIndex,0);
		std::cout << "BVH Nodes: " << bvhNodes.size() << std::endl;

	}*/

	//void Mesh::BuildBVHRecursive(int start, int end, int& currentIndex, int depth) {
	//	BVHNode node;
	//	BoundingBox bbox;

	//	// Initialize bounding box to extreme values
	//	bbox.min = glm::vec3(std::numeric_limits<float>::max());
	//	bbox.max = glm::vec3(std::numeric_limits<float>::lowest());

	//	// Compute bounding box for this node
	//	for (int i = start; i < end; ++i) {
	//		int idx = indices[i];
	//		if (idx < 0 || idx >= vertices.size()) {
	//			std::cerr << "Invalid index: " << idx << " (vertices size: " << vertices.size() << ")" << std::endl;
	//			continue; // Skip invalid indices
	//		}

	//		const glm::vec3& vertexPos = vertices[idx].Position;
	//		bbox.min = glm::min(bbox.min, vertexPos);
	//		bbox.max = glm::max(bbox.max, vertexPos);
	//	}

	//	std::cout << "BVH Node BoundingBox min: (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ")" << std::endl;
	//	std::cout << "BVH Node BoundingBox max: (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")" << std::endl;

	//	node.bbox = bbox;

	//	// If this is a leaf node or max depth reached
	//	if (end - start <= 2 || depth >= 16) {
	//		node.triangleIndices.assign(indices.begin() + start, indices.begin() + end);
	//		bvhNodes.push_back(node);
	//		currentIndex = bvhNodes.size() - 1;
	//		return;
	//	}

	//	// Split using SAH (Surface Area Heuristic)
	//	int bestAxis = -1;
	//	int bestIndex = -1;
	//	float bestCost = std::numeric_limits<float>::max();

	//	for (int axis = 0; axis < 3; ++axis) {
	//		// Sort triangles along the axis
	//		std::sort(indices.begin() + start, indices.begin() + end, [this, axis](int a, int b) {
	//			return vertices[a].Position[axis] < vertices[b].Position[axis];
	//			});

	//		// Evaluate SAH cost for all splits
	//		for (int i = start + 1; i < end; ++i) {
	//			BoundingBox leftBox, rightBox;

	//			// Compute left bounding box
	//			for (int j = start; j < i; ++j) {
	//				int idx = indices[j];
	//				if (idx < 0 || idx >= vertices.size()) {
	//					std::cerr << "Invalid index: " << idx << " (vertices size: " << vertices.size() << ")" << std::endl;
	//					continue; // Skip invalid indices
	//				}
	//				leftBox.min = glm::min(leftBox.min, vertices[idx].Position);
	//				leftBox.max = glm::max(leftBox.max, vertices[idx].Position);
	//			}

	//			// Compute right bounding box
	//			for (int j = i; j < end; ++j) {
	//				int idx = indices[j];
	//				if (idx < 0 || idx >= vertices.size()) {
	//					std::cerr << "Invalid index: " << idx << " (vertices size: " << vertices.size() << ")" << std::endl;
	//					continue; // Skip invalid indices
	//				}
	//				rightBox.min = glm::min(rightBox.min, vertices[idx].Position);
	//				rightBox.max = glm::max(rightBox.max, vertices[idx].Position);
	//			}

	//			// Calculate SAH cost
	//			float cost = SAH(leftBox, i - start) + SAH(rightBox, end - i);
	//			if (cost < bestCost) {
	//				bestCost = cost;
	//				bestAxis = axis;
	//				bestIndex = i;
	//			}
	//		}
	//	}

	//	if (bestAxis != -1) {
	//		// Sort along the best axis
	//		std::sort(indices.begin() + start, indices.begin() + end, [this, bestAxis](int a, int b) {
	//			return vertices[a].Position[bestAxis] < vertices[b].Position[bestAxis];
	//			});

	//		// Recursively build left and right children
	//		node.leftChild = bvhNodes.size();
	//		BuildBVHRecursive(start, bestIndex, currentIndex, depth + 1);

	//		node.rightChild = bvhNodes.size();
	//		BuildBVHRecursive(bestIndex, end, currentIndex, depth + 1);
	//	}

	//	bvhNodes.push_back(node);
	//	currentIndex = bvhNodes.size() - 1;
	//	std::cout << "Node created at index " << currentIndex << " with leftChild: " << node.leftChild << " and rightChild: " << node.rightChild << std::endl;

	//}

	//bool Mesh::Raycast(const Ray& ray, float& t) const {
	//	if (bvhNodes.empty()) {
	//		std::cerr << "BVH is empty!" << std::endl;
	//		return false; // No BVH nodes
	//	}
	//	std::vector<int> stack = { 0 };
	//	bool hit = false;

	//	while (!stack.empty()) {
	//		int nodeIndex = stack.back();
	//		stack.pop_back();

	//		if (nodeIndex < 0 || nodeIndex >= bvhNodes.size()) {
	//			std::cerr << "Invalid node index: " << nodeIndex << std::endl;
	//			continue; // Skip invalid nodes
	//		}

	//		const BVHNode& node = bvhNodes[nodeIndex];
	//		float tMin, tMax;

	//		if (!node.bbox.Intersect(ray, tMin, tMax)) {
	//			continue; // Skip if no intersection
	//		}

	//		if (!node.triangleIndices.empty()) {
	//			for (int triIndex : node.triangleIndices) {
	//				if (triIndex < 0 || triIndex >= indices.size() / 3) {
	//					std::cerr << "Invalid triangle index: " << triIndex << std::endl;
	//					continue; // Skip invalid triangles
	//				}

	//				const glm::vec3& v0 = vertices[indices[triIndex * 3]].Position;
	//				const glm::vec3& v1 = vertices[indices[triIndex * 3 + 1]].Position;
	//				const glm::vec3& v2 = vertices[indices[triIndex * 3 + 2]].Position;

	//				if (IntersectTriangle(ray, v0, v1, v2, t)) {
	//					hit = true;
	//					break; // Exit once a hit is found
	//				}
	//			}
	//		}
	//		else {
	//			if (node.leftChild >= 0 && node.leftChild < bvhNodes.size()) {
	//				stack.push_back(node.leftChild);
	//			}
	//			if (node.rightChild >= 0 && node.rightChild < bvhNodes.size()) {
	//				stack.push_back(node.rightChild);
	//			}
	//		}
	//	}

	//	return hit;
	//}

	//bool Mesh::IntersectTriangle(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) const {
	//	glm::vec3 edge1 = v1 - v0;
	//	glm::vec3 edge2 = v2 - v0;
	//	glm::vec3 h = glm::cross(ray.direction, edge2);
	//	float a = glm::dot(edge1, h);

	//	if (a > -1e-6 && a < 1e-6) return false; // Ray parallel to triangle

	//	float f = 1.0 / a;
	//	glm::vec3 s = ray.origin - v0;
	//	float u = f * glm::dot(s, h);

	//	if (u < 0.0 || u > 1.0) return false;

	//	glm::vec3 q = glm::cross(s, edge1);
	//	float v = f * glm::dot(ray.direction, q);

	//	if (v < 0.0 || u + v > 1.0) return false;

	//	t = f * glm::dot(edge2, q);
	//	return t > 1e-6;
	//}
	//float Mesh::SurfaceArea(const BoundingBox& bbox) const {
	//	glm::vec3 extent = bbox.max - bbox.min;
	//	return 2.0f * (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x);
	//}

	//float Mesh::SAH(const BoundingBox& bbox, int numTriangles) const {
	//	const float Ct = 1.0f; // Cost of traversing a node
	//	const float Ci = 2.0f; // Cost of intersecting a primitive

	//	float surfaceArea = SurfaceArea(bbox);
	//	return Ct + Ci * surfaceArea * numTriangles;
	//}




}
