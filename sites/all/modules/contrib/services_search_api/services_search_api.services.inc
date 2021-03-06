<?php

/**
 * Implementation of hook_services_resources().
 */
function services_search_api_services_resources() {
  return array(
    'search_api' => array(
      'retrieve' => array(
        'file' => array('type' => 'inc', 'module' => 'services_search_api', 'name' => 'services_search_api.resources'),
        'callback' => '_services_search_api_resource_retrieve',
        'args' => array(
          array(
            'name' => 'index',
            'type' => 'string',
            'optional' => FALSE,
            'source' => array('path' => '0'),
            'description' => t('Search API index machine name to use.'),
          ),
          array(
            'name' => 'keys',
            'type' => 'string',
            'optional' => TRUE,
            'source' => array('param' => 'keys'),
            'default value' => '',
            'description' => t('Search terms.'),
          ),
          array(
            'name' => 'filter',
            'type' => 'array',
            'optional' => TRUE,
            'source' => array('param' => 'filter'),
            'default value' => '',
            'description' => t('String of filters to apply to the search, in the form \'filter[~and][title]=title\'. Filters can be nested.'),
          ),
          array(
            'name' => 'sort',
            'type' => 'array',
            'optional' => TRUE,
            'source' => array('param' => 'sort'),
            'default value' => '',
            'description' => t('String containing the sort elements for the search in the form \'sort[nid]=DESC\'. Multiple sort terms can be used.'),
          ),
          array(
            'name' => 'limit',
            'type' => 'int',
            'optional' => TRUE,
            'source' => array('param' => 'limit'),
            'description' => t('Number of results to retrieve.'),
          ),
          array(
            'name' => 'offset',
            'type' => 'int',
            'optional' => TRUE,
            'source' => array('param' => 'offset'),
            'default value' => 0,
            'description' => t('The position of the first returned search result.'),
          ),
          array(
            'name' => 'fields',
            'optional' => TRUE,
            'type' => 'array',
            'description' => 'The fields to get, if fields are from a related entity, use field_relationship:field_name for getting a given field or field_relationship_* for getting the whole set of data',
            'default value' => array(),
            'source' => array('param' => 'fields'),
          ),
        ),
        'access arguments' => array('services_search_api search from any index'),
        'access arguments append' => FALSE,
      ),
    ),
  );
}
