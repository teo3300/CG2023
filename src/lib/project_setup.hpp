#ifndef PROJECT_SETUP_HPP
#define PROJECT_SETUP_HPP

// This has been adapted from the Vulkan tutorial

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <array>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <chrono>

#include <tiny_obj_loader.h>

#include <stb_image.h>

#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#include <tiny_gltf.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const int MAX_FRAMES_IN_FLIGHT = 2;

extern const std::vector<const char*> validationLayers;

extern std::vector<const char*> deviceExtensions;

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	inline bool isComplete() {
		return graphicsFamily.has_value() &&
			   presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

struct errorcode {
	VkResult resultCode;
	std::string meaning;
};
extern struct errorcode ErrorCodes[];

void PrintVkError( VkResult result );

class BaseProject;

struct VertexBindingDescriptorElement {
	uint32_t binding;
	uint32_t stride;
	VkVertexInputRate inputRate;
};

enum VertexDescriptorElementUsage {POSITION, NORMAL, UV, COLOR, TANGENT, OTHER};

struct VertexDescriptorElement {
	uint32_t binding;
	uint32_t location;
	VkFormat format;
	uint32_t offset;
	uint32_t size;
	VertexDescriptorElementUsage usage;
};

struct VertexComponent {
	bool hasIt;
	uint32_t offset;
};

struct VertexDescriptor {
	BaseProject *BP;
	
	VertexComponent Position;
	VertexComponent Normal;
	VertexComponent UV;
	VertexComponent Color;
	VertexComponent Tangent;

	std::vector<VertexBindingDescriptorElement> Bindings;
	std::vector<VertexDescriptorElement> Layout;
 	
 	void init(BaseProject *bp, std::vector<VertexBindingDescriptorElement> B, std::vector<VertexDescriptorElement> E);
	void cleanup();

	std::vector<VkVertexInputBindingDescription> getBindingDescription();
	std::vector<VkVertexInputAttributeDescription>
						getAttributeDescriptions();
};

enum ModelType {OBJ, GLTF};

template <class Vert>
class Model {
	BaseProject *BP;
	
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VertexDescriptor *VD;

	public:
	std::vector<Vert> vertices{};
	std::vector<uint32_t> indices{};
	void loadModelOBJ(std::string file);
	void loadModelGLTF(std::string file);
	void createIndexBuffer();
	void createVertexBuffer();

	void init(BaseProject *bp, VertexDescriptor *VD, std::string file, ModelType MT);
	void initMesh(BaseProject *bp, VertexDescriptor *VD);
	void cleanup();
  	void bind(VkCommandBuffer commandBuffer);
};

struct Texture {
	BaseProject *BP;
	uint32_t mipLevels;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
	int imgs;
	static const int maxImgs = 6;
	
	void createTextureImage(const char *const files[], VkFormat Fmt);
	void createTextureImageView(VkFormat Fmt);
	void createTextureSampler(VkFilter magFilter,
							 VkFilter minFilter,
							 VkSamplerAddressMode addressModeU,
							 VkSamplerAddressMode addressModeV,
							 VkSamplerMipmapMode mipmapMode,
							 VkBool32 anisotropyEnable,
							 float maxAnisotropy,
							 float maxLod
							);

	void init(BaseProject *bp, const char * file, VkFormat Fmt = VK_FORMAT_R8G8B8A8_SRGB, bool initSampler = true);
	void initCubic(BaseProject *bp, const char * files[6]);
	void cleanup();
};

struct DescriptorSetLayoutBinding {
	uint32_t binding;
	VkDescriptorType type;
	VkShaderStageFlags flags;
};


struct DescriptorSetLayout {
	BaseProject *BP;
 	VkDescriptorSetLayout descriptorSetLayout;
 	
 	void init(BaseProject *bp, std::vector<DescriptorSetLayoutBinding> B);
	void cleanup();
};

struct Pipeline {
	BaseProject *BP;
	VkPipeline graphicsPipeline;
  	VkPipelineLayout pipelineLayout;
 
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	std::vector<DescriptorSetLayout *> D;	
	
	VkCompareOp compareOp;
	VkPolygonMode polyModel;
 	VkCullModeFlagBits CM;
 	bool transp;
	
	VertexDescriptor *VD;
  	
  	void init(BaseProject *bp, VertexDescriptor *vd,
			  const std::string& VertShader, const std::string& FragShader,
  			  std::vector<DescriptorSetLayout *> D);
  	void setAdvancedFeatures(VkCompareOp _compareOp, VkPolygonMode _polyModel,
 						VkCullModeFlagBits _CM, bool _transp);
  	void create();
  	void destroy();
  	void bind(VkCommandBuffer commandBuffer);
  	
  	VkShaderModule createShaderModule(const std::vector<char>& code);
  	static std::vector<char> readFile(const std::string& filename);  	
	void cleanup();
};

enum DescriptorSetElementType {UNIFORM, TEXTURE};

struct DescriptorSetElement {
	int binding;
	DescriptorSetElementType type;
	int size;
	Texture *tex;
};

struct DescriptorSet {
	BaseProject *BP;

	std::vector<std::vector<VkBuffer>> uniformBuffers;
	std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory;
	std::vector<VkDescriptorSet> descriptorSets;
	
	std::vector<bool> toFree;

	void init(BaseProject *bp, DescriptorSetLayout *L,
		std::vector<DescriptorSetElement> E);
	void cleanup();
  	void bind(VkCommandBuffer commandBuffer, Pipeline &P, int setId, int currentImage);
  	void map(int currentImage, void *src, int size, int slot);
};


// MAIN ! 
class BaseProject {
	friend struct VertexDescriptor;
	template <class Vert> friend class Model;
	friend struct Texture;
	friend struct Pipeline;
	friend struct DescriptorSetLayout;
	friend struct DescriptorSet;
public:
	virtual void setWindowParameters() = 0;
    void run();

protected:
	uint32_t windowWidth;
	uint32_t windowHeight;
	bool windowResizable;
	std::string windowTitle;
	VkClearColorValue initialBackgroundColor;
	int uniformBlocksInPool;
	int texturesInPool;
	int setsInPool;

    GLFWwindow* window;
    VkInstance instance;

	VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	
	VkRenderPass renderPass;
	
 	VkDescriptorPool descriptorPool;

	VkDebugUtilsMessengerEXT debugMessenger;
	
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	size_t currentFrame = 0;
	bool framebufferResized = false;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	
    void initWindow();

	virtual void onWindowResize(int w, int h) = 0;
	
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<BaseProject*>
						(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
		app->onWindowResize(width, height);
	} 
	

	virtual void localInit() = 0;
	virtual void pipelinesAndDescriptorSetsInit() = 0;

    void initVulkan();

    void createInstance();
    
    std::vector<const char*> getRequiredExtensions();
	
	inline bool checkIfItHasExtension(const char *ext);
	
	bool checkIfItHasDeviceExtension(VkPhysicalDevice device, const char *ext);
	
	bool checkValidationLayerSupport();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;		
		return VK_FALSE;
	}

	void setupDebugMessenger();

    void createSurface();

	class deviceReport {
		public:
			bool swapChainAdequate;
			bool swapChainFormatSupport;
			bool swapChainPresentModeSupport;
			bool completeQueueFamily;
			bool anisotropySupport;
			bool extensionsSupported;
			std::set<std::string> requiredExtensions;
			
			void print();
	};

    void pickPhysicalDevice();
	
    bool isDeviceSuitable(VkPhysicalDevice device, deviceReport &devRep);
    
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	bool checkDeviceExtensionSupport(VkPhysicalDevice device, deviceReport &devRep);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	VkSampleCountFlagBits getMaxUsableSampleCount();

	void createLogicalDevice();
	
	void createSwapChain();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(
				const std::vector<VkSurfaceFormatKHR>& availableFormats);
	

	VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes);
	
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImageViews();
	
	VkImageView createImageView(VkImage image, VkFormat format,
								VkImageAspectFlags aspectFlags,
								uint32_t mipLevels, VkImageViewType type, int layerCount
								);
	
	void createRenderPass();

    void createFramebuffers();

    void createCommandPool();

	void createColorResources();

	void createDepthResources();

	VkFormat findDepthFormat();
	
	VkFormat findSupportedFormat(const std::vector<VkFormat> candidates,
						VkImageTiling tiling, VkFormatFeatureFlags features);
	
	static inline bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			   format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
		
	void createImage(uint32_t width, uint32_t height,
					 uint32_t mipLevels, int imgCount,
					 VkSampleCountFlagBits numSamples, 
					 VkFormat format,
				 	 VkImageTiling tiling, VkImageUsageFlags usage,
				 	 VkImageCreateFlags cflags,
				 	 VkMemoryPropertyFlags properties, VkImage& image,
				 	 VkDeviceMemory& imageMemory);

	void generateMipmaps(VkImage image, VkFormat imageFormat,
						 int32_t texWidth, int32_t texHeight,
						 uint32_t mipLevels, int layerCount);
	
	void transitionImageLayout(VkImage image, VkFormat format,
					VkImageLayout oldLayout, VkImageLayout newLayout,
					uint32_t mipLevels, int layersCount);
	
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t
						   width, uint32_t height, int layerCount);
	
	VkCommandBuffer beginSingleTimeCommands();
	
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties,
					  VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	
	uint32_t findMemoryType(uint32_t typeFilter,
							VkMemoryPropertyFlags properties);
    
	void createDescriptorPool();
	
	virtual void populateCommandBuffer(VkCommandBuffer commandBuffer, int i) = 0;

    void createCommandBuffers();
    
    void createSyncObjects();
	
    void mainLoop();
    
    void drawFrame();

	virtual void updateUniformBuffer(uint32_t currentImage) = 0;

	virtual void pipelinesAndDescriptorSetsCleanup() = 0;
	virtual void localCleanup() = 0;
	
    void recreateSwapChain();

	void cleanupSwapChain();
		
    void cleanup();
	
	inline void RebuildPipeline() {
		framebufferResized = true;
	}
	
	
	// Control Wrapper
	void handleGamePad(int id,  glm::vec3 &m, glm::vec3 &r, bool &fire);
		
	void getSixAxis(float &deltaT, glm::vec3 &m, glm::vec3 &r, bool &fire);
	
	// Public part of the base class
	public:
	// Debug commands
	static inline void printFloat(const char *Name, float v) {
		std::cout << "float " << Name << " = " << v << ";\n";
	}
	static inline void printVec2(const char *Name, glm::vec2 v) {
		std::cout << "glm::vec3 " << Name << " = glm::vec3(" << v[0] << ", " << v[1] << ");\n";
	}
	static inline void printVec3(const char *Name, glm::vec3 v) {
		std::cout << "glm::vec3 " << Name << " = glm::vec3(" << v[0] << ", " << v[1] << ", " << v[2] << ");\n";
	}
	static inline void printVec4(const char *Name, glm::vec4 v) {
		std::cout << "glm::vec4 " << Name << " = glm::vec4(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ");\n";
	}
	static inline void printMat3(const char *Name, glm::mat3 v) {
			std::cout << "glm::mat3 " << Name << " = glm::mat3(";
			for(int i = 0; i<9; i++) {
				std::cout << v[i/3][i%3] << ((i<8) ? ", " : ");\n");
			}
	}
	static inline void printMat4(const char *Name, glm::mat4 v) {
			std::cout << "glm::mat4 " << Name << " = glm::mat4(";
			for(int i = 0; i<16; i++) {
				std::cout << v[i/4][i%4] << ((i<15) ? ", " : ");\n");
			}
	}
};




// Helper classes
template <class Vert>
void Model<Vert>::loadModelOBJ(std::string file) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;
	
	std::cout << "Loading : " << file << "[OBJ]\n";	
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
						  file.c_str())) {
		throw std::runtime_error(warn + err);
	}
	
	std::cout << "Building\n";	
//	std::cout << "Position " << VD->Position.hasIt << "," << VD->Position.offset << "\n";	
//	std::cout << "UV " << VD->UV.hasIt << "," << VD->UV.offset << "\n";	
//	std::cout << "Normal " << VD->Normal.hasIt << "," << VD->Normal.offset << "\n";	
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vert vertex{};
			glm::vec3 pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			if(VD->Position.hasIt) {
				glm::vec3 *o = (glm::vec3 *)((char*)(&vertex) + VD->Position.offset);
				*o = pos;
			}
			
			glm::vec3 color = {
				attrib.colors[3 * index.vertex_index + 0],
				attrib.colors[3 * index.vertex_index + 1],
				attrib.colors[3 * index.vertex_index + 2]
			};
			if(VD->Color.hasIt) {
				glm::vec3 *o = (glm::vec3 *)((char*)(&vertex) + VD->Color.offset);
				*o = color;
			}
			
			glm::vec2 texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1 - attrib.texcoords[2 * index.texcoord_index + 1] 
			};
			if(VD->UV.hasIt) {
				glm::vec2 *o = (glm::vec2 *)((char*)(&vertex) + VD->UV.offset);
				*o = texCoord;
			}

			glm::vec3 norm = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};
			if(VD->Normal.hasIt) {
				glm::vec3 *o = (glm::vec3 *)((char*)(&vertex) + VD->Normal.offset);
				*o = norm;
			}
			
			vertices.push_back(vertex);
			indices.push_back(vertices.size()-1);
		}
	}
	std::cout << "[OBJ] Vertices: "<< vertices.size() << "\n";
	std::cout << "Indices: "<< indices.size() << "\n";
	
}

template <class Vert>
void Model<Vert>::loadModelGLTF(std::string file) {
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string warn, err;
	
	std::cout << "Loading : " << file << "[GLTF]\n";	
	if (!loader.LoadASCIIFromFile(&model, &warn, &err, 
					file.c_str())) {
		throw std::runtime_error(warn + err);
	}
	
	for (const auto& mesh :  model.meshes) {
		std::cout << "Primitives: " << mesh.primitives.size() << "\n";
		for (const auto& primitive :  mesh.primitives) {
			if (primitive.indices < 0) {
				continue;
			}

			const float *bufferPos = nullptr;
			const float *bufferNormals = nullptr;
			const float *bufferTangents = nullptr;
			const float *bufferTexCoords = nullptr;
			
			bool meshHasPos = false;
			bool meshHasNorm = false;
			bool meshHasTan = false;
			bool meshHasUV = false;
			
			int cntPos = 0;
			int cntNorm = 0;
			int cntTan = 0;
			int cntUV = 0;
			int cntTot = 0;
			
			auto pIt = primitive.attributes.find("POSITION");
			if(pIt != primitive.attributes.end()) {
				const tinygltf::Accessor &posAccessor = model.accessors[pIt->second];
				const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				meshHasPos = true;
				cntPos = posAccessor.count;
				if(cntPos > cntTot) cntTot = cntPos;
			} else {
				if(VD->Position.hasIt) {
					std::cout << "Warning: vertex layout has position, but file hasn't\n";
				}
			}
			
			auto nIt = primitive.attributes.find("NORMAL");
			if(nIt != primitive.attributes.end()) {
				const tinygltf::Accessor &normAccessor = model.accessors[nIt->second];
				const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
				bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
				meshHasNorm = true;
				cntNorm = normAccessor.count;
				if(cntNorm > cntTot) cntTot = cntNorm;
			} else {
				if(VD->Normal.hasIt) {
					std::cout << "Warning: vertex layout has normal, but file hasn't\n";
				}
			}

			auto tIt = primitive.attributes.find("TANGENT");
			if(tIt != primitive.attributes.end()) {
				const tinygltf::Accessor &tanAccessor = model.accessors[tIt->second];
				const tinygltf::BufferView &tanView = model.bufferViews[tanAccessor.bufferView];
				bufferTangents = reinterpret_cast<const float *>(&(model.buffers[tanView.buffer].data[tanAccessor.byteOffset + tanView.byteOffset]));
				meshHasTan = true;
				cntTan = tanAccessor.count;
				if(cntTan > cntTot) cntTot = cntTan;
			} else {
				if(VD->Tangent.hasIt) {
					std::cout << "Warning: vertex layout has tangent, but file hasn't\n";
				}
			}

			auto uIt = primitive.attributes.find("TEXCOORD_0");
			if(uIt != primitive.attributes.end()) {
				const tinygltf::Accessor &uvAccessor = model.accessors[uIt->second];
				const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
				bufferTexCoords = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				meshHasUV = true;
				cntUV = uvAccessor.count;
				if(cntUV > cntTot) cntTot = cntUV;
			} else {
				if(VD->UV.hasIt) {
					std::cout << "Warning: vertex layout has UV, but file hasn't\n";
				}
			}
			
			for(int i = 0; i < cntTot; i++) {
				Vert vertex{};
				
				if((i < cntPos) && meshHasPos && VD->Position.hasIt) {
					glm::vec3 pos = {
						bufferPos[3 * i + 0],
						bufferPos[3 * i + 1],
						bufferPos[3 * i + 2]
					};
					glm::vec3 *o = (glm::vec3 *)((char*)(&vertex) + VD->Position.offset);
					*o = pos;
				}
	
				if((i < cntNorm) && meshHasNorm && VD->Normal.hasIt) {
					glm::vec3 normal = {
						bufferNormals[3 * i + 0],
						bufferNormals[3 * i + 1],
						bufferNormals[3 * i + 2]
					};
					glm::vec3 *o = (glm::vec3 *)((char*)(&vertex) + VD->Normal.offset);
					*o = normal;
				}

				if((i < cntTan) && meshHasTan && VD->Tangent.hasIt) {
					glm::vec4 tangent = {
						bufferTangents[4 * i + 0],
						bufferTangents[4 * i + 1],
						bufferTangents[4 * i + 2],
						bufferTangents[4 * i + 3]
					};
					glm::vec4 *o = (glm::vec4 *)((char*)(&vertex) + VD->Tangent.offset);
					*o = tangent;
				}
				
				if((i < cntUV) && meshHasUV && VD->UV.hasIt) {
					glm::vec2 texCoord = {
						bufferTexCoords[2 * i + 0],
						bufferTexCoords[2 * i + 1] 
					};
					glm::vec2 *o = (glm::vec2 *)((char*)(&vertex) + VD->UV.offset);
					*o = texCoord;
				}

				vertices.push_back(vertex);					
			} 

			const tinygltf::Accessor &accessor = model.accessors[primitive.indices];
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			
			switch(accessor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
					{
						const uint16_t *bufferIndex = reinterpret_cast<const uint16_t *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
						for(int i = 0; i < accessor.count; i++) {
							indices.push_back(bufferIndex[i]);
						}
					}
					break;
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
					{
						const uint32_t *bufferIndex = reinterpret_cast<const uint32_t *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
						for(int i = 0; i < accessor.count; i++) {
							indices.push_back(bufferIndex[i]);
						}
					}
					break;
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					throw std::runtime_error("Error loading GLTF component");			
			}
		}
	}

	std::cout << "[GLTF] Vertices: " << vertices.size()
			  << "\nIndices: " << indices.size() << "\n";
}

template <class Vert>
void Model<Vert>::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	BP->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						vertexBuffer, vertexBufferMemory);

	void* data;
	vkMapMemory(BP->device, vertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(BP->device, vertexBufferMemory);			
}

template <class Vert>
void Model<Vert>::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	BP->createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
							 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							 indexBuffer, indexBufferMemory);

	void* data;
	vkMapMemory(BP->device, indexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t) bufferSize);
	vkUnmapMemory(BP->device, indexBufferMemory);
}

template <class Vert>
void Model<Vert>::initMesh(BaseProject *bp, VertexDescriptor *vd) {
	BP = bp;
	VD = vd;
	std::cout << "[Manual] Vertices: " << vertices.size()
			  << "\nIndices: " << indices.size() << "\n";
	createVertexBuffer();
	createIndexBuffer();
}

template <class Vert>
void Model<Vert>::init(BaseProject *bp, VertexDescriptor *vd, std::string file, ModelType MT) {
	BP = bp;
	VD = vd;
	if(MT == OBJ) {
		loadModelOBJ(file);
	} else if(MT == GLTF) {
		loadModelGLTF(file);
	}
	createVertexBuffer();
	createIndexBuffer();
}

template <class Vert>
void Model<Vert>::cleanup() {
   	vkDestroyBuffer(BP->device, indexBuffer, nullptr);
   	vkFreeMemory(BP->device, indexBufferMemory, nullptr);
	vkDestroyBuffer(BP->device, vertexBuffer, nullptr);
   	vkFreeMemory(BP->device, vertexBufferMemory, nullptr);
}

template <class Vert>
void Model<Vert>::bind(VkCommandBuffer commandBuffer) {
	VkBuffer vertexBuffers[] = {vertexBuffer};
	// property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	// property .indexBuffer of models, contains the VkBuffer handle to its index buffer
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0,
							VK_INDEX_TYPE_UINT32);
}

#endif//PROJECT_SETUP_HPP