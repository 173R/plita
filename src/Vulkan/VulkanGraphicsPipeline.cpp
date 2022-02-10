#include "VulkanGraphicsPipeline.hpp"
#include <iostream>

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* device, VulkanSwapChain* swap_chain):
  vk_graphics_pipeline_(VK_NULL_HANDLE),
  vk_render_pass_(VK_NULL_HANDLE),
  vk_pipeline_layout_(VK_NULL_HANDLE),
  device_(device),
  swap_chain_(swap_chain),
  vk_command_pool_(VK_NULL_HANDLE),
  vk_image_available_semaphore_(VK_NULL_HANDLE),
  vk_render_finished_semaphore_(VK_NULL_HANDLE)
{

}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
  vkDeviceWaitIdle(device_->vk_device_);
  vkDestroySemaphore(device_->vk_device_, vk_render_finished_semaphore_, nullptr);
  vkDestroySemaphore(device_->vk_device_, vk_image_available_semaphore_, nullptr);
  vkDestroyCommandPool(device_->vk_device_, vk_command_pool_, nullptr);
  for (auto framebuffer : vk_framebuffers_) {
    vkDestroyFramebuffer(device_->vk_device_, framebuffer, nullptr);
  }
  vkDestroyPipeline(device_->vk_device_, vk_graphics_pipeline_, nullptr);
  vkDestroyPipelineLayout(device_->vk_device_, vk_pipeline_layout_, nullptr);
  vkDestroyRenderPass(device_->vk_device_, vk_render_pass_, nullptr);
}


/*static*/ std::vector<char> VulkanGraphicsPipeline::readFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }
  int64_t file_size = (int64_t)file.tellg();
  std::vector<char> buffer(file_size);
  file.seekg(0);
  file.read(buffer.data(), file_size);
  file.close();

  return buffer;
}

void VulkanGraphicsPipeline::createPipeline() {

  createRenderPass();

  std::vector<char> vert_shader_code = readFile("C:/C++WorkSpace/plita/src/shaders/vert.spv");
  std::vector<char> frag_shader_code = readFile("C:/C++WorkSpace/plita/src/shaders/frag.spv");

  std::cout << vert_shader_code.size() << "\n" << frag_shader_code.size() << "\n";

  VkShaderModule vert_shader_module = createShaderModule(vert_shader_code);
  VkShaderModule frag_shader_module = createShaderModule(frag_shader_code);

  /*Назначение созданных шейдеров для определённой части конвейера*/
  VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

  /*Описываем формат входных данных в вершинный шейдер*/
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;

  /*Способ отрисовки вершин*/
  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  /*Описываем область вывода*/
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swap_chain_->vk_image_extent_.width);
  viewport.height = static_cast<float>(swap_chain_->vk_image_extent_.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  /*Обрезка области вывода, в данном случае покрывает весь viewport, ничего не обрезая*/
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swap_chain_->vk_image_extent_;

  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  /*Растеризатор*/
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  /*Мультисемплинг, пока отключён*/
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  /*Смешивание цветов для буфера кадров*/
  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
    | VK_COLOR_COMPONENT_G_BIT
    | VK_COLOR_COMPONENT_B_BIT
    | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  /*Смешивание цветов, глобальная настройка*/
  VkPipelineColorBlendStateCreateInfo color_blending{};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f; // Optional
  color_blending.blendConstants[1] = 0.0f; // Optional
  color_blending.blendConstants[2] = 0.0f; // Optional
  color_blending.blendConstants[3] = 0.0f; // Optional

  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 0; // Optional
  pipeline_layout_info.pushConstantRangeCount = 0; // Optional

  if (vkCreatePipelineLayout(device_->vk_device_, &pipeline_layout_info, nullptr, &vk_pipeline_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkGraphicsPipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertexInputInfo;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.layout = vk_pipeline_layout_;
  pipeline_info.renderPass = vk_render_pass_;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional

  if (vkCreateGraphicsPipelines(device_->vk_device_, VK_NULL_HANDLE, 1,
                                &pipeline_info,nullptr, &vk_graphics_pipeline_) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  vkDestroyShaderModule(device_->vk_device_, vert_shader_module, nullptr);
  vkDestroyShaderModule(device_->vk_device_, frag_shader_module, nullptr);

  /*TODO: Перенести*/
  createFramebuffers();
  createCommandPool();
  createCommandBuffers();
  createSemaphores();
}

VkShaderModule VulkanGraphicsPipeline::createShaderModule(const std::vector<char> &code) const {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shader_module;
  if (vkCreateShaderModule(device_->vk_device_, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shader_module;
}

void VulkanGraphicsPipeline::createRenderPass() {

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription color_attachment{};
  color_attachment.format = swap_chain_->vk_image_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(device_->vk_device_, &render_pass_info, nullptr, &vk_render_pass_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void VulkanGraphicsPipeline::createFramebuffers() {
  vk_framebuffers_.resize(swap_chain_->vk_images_.size());

  for (size_t i = 0; i < swap_chain_->vk_image_views_.size(); i++) {
    VkImageView attachments[] = {
      swap_chain_->vk_image_views_[i]
    };

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = vk_render_pass_;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = swap_chain_->vk_image_extent_.width;
    framebuffer_info.height = swap_chain_->vk_image_extent_.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(device_->vk_device_, &framebuffer_info, nullptr, &vk_framebuffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void VulkanGraphicsPipeline::createCommandPool() {
  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = device_->queue_indices_.graphics_family.value();

  if (vkCreateCommandPool(device_->vk_device_, &pool_info, nullptr, &vk_command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void VulkanGraphicsPipeline::createCommandBuffers() {
  vk_command_buffers_.resize(vk_framebuffers_.size());

  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = vk_command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = static_cast<uint32_t>(vk_command_buffers_.size());

  if (vkAllocateCommandBuffers(device_->vk_device_, &alloc_info, vk_command_buffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  for (size_t i = 0; i < vk_command_buffers_.size(); i++) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(vk_command_buffers_[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = vk_render_pass_;
    render_pass_info.framebuffer = vk_framebuffers_[i];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swap_chain_->vk_image_extent_;

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(vk_command_buffers_[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(vk_command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphics_pipeline_);

    vkCmdDraw(vk_command_buffers_[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(vk_command_buffers_[i]);

    if (vkEndCommandBuffer(vk_command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}

void VulkanGraphicsPipeline::createSemaphores() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(device_->vk_device_, &semaphoreInfo, nullptr, &vk_image_available_semaphore_) != VK_SUCCESS ||
      vkCreateSemaphore(device_->vk_device_, &semaphoreInfo, nullptr, &vk_render_finished_semaphore_) != VK_SUCCESS) {

    throw std::runtime_error("failed to create semaphores!");
  }
}
