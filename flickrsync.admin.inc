<?php

/**
 * @file
 * Administrative page callbacks for the flickrsync module.
 */


/**
 * Implementation of hook_settings().
 */
function flickrsync_admin_settings() {
  $node_types = node_get_types();
  $types[] = '<none>';
  foreach ($node_types as $type => $obj) {
    $types[$type] = $obj->name;
  }
  $form['flickrsync_node_type'] = array(
    '#type' => 'select',
    '#title' => t('Node type to use with Flickr Sync'),
    '#default_value' => variable_get('flickrsync_node_type', ''),
    '#options' => $types,
    '#description' => t('You must select a node type that has an imagefield for the Flickr image download and optionally a free-tagging taxonomy vocabulary.'),
  );
  if ($type = variable_get('flickrsync_node_type', '')) {
    //$fields = content_fields(NULL, $type); // doesn't seem to work as expected
    $info = _content_type_info();
    $fields = $info['content types'][$type]['fields'];
    $choices = array(
      'text' => array('body' => 'Body'),
      'content_taxonomy' => array('default' => '<none>'),
			'date' => array('created' => '<none>')
    );
    foreach ($fields as $name => $field) {
      $module = $field['widget']['module'];
      if (in_array($module, array('imagefield', 'text', 'openlayers_cck', 'date'))) {
        $choices[$module][$name] = $field['widget']['label'];
      } elseif (substr($module, 0, 16) == 'content_taxonomy') {
        $choices['content_taxonomy'][$name] = $field['widget']['label'];
      }			
    }
		
		if ($choices['imagefield']) {
			$form['flickrsync_image_field'] = array(
        '#type' => 'select',
        '#title' => t('Image field for attaching Flickr photo'),
    		'#default_value' => variable_get('flickrsync_image_field', ''),
        '#options' => $choices['imagefield'],
        '#description' => t('Select the image field to use for attaching Flickr images to Drupal nodes.'),
      );
		} else {
			drupal_set_message("The node type selected doesn't have any text fields.  Please select a different node type, or add an text field to this node type.", 'error');
		}
		if ($choices['text']) {
			$form['flickrsync_text_field'] = array(
				'#type' => 'select',
				'#title' => t('Text field for attaching Flickr Description'),
				'#default_value' => variable_get('flickrsync_text_field', ''),
				'#options' => $choices['text'],
				'#description' => t('Select the text field to use for attaching Flickr text to Drupal nodes.'),
			);
		} else {
			drupal_set_message("The node type selected doesn't have any text fields.  Please select a different node type, or add an text field to this node type.", 'error');
		}
		if ($choices['content_taxonomy']) {
			$form['flickrsync_content_taxonomy'] = array(
				'#type' => 'select',
				'#title' => t('Taxonomy field for attaching Tags Description'),
				'#default_value' => variable_get('flickrsync_content_taxonomy', ''),
				'#options' => $choices['content_taxonomy'],
				'#description' => t('Select the taxonomy field to use for attaching Flickr tags to Drupal nodes.'),
			);
		} else {
			drupal_set_message("The node type selected doesn't have any taxonomy fields.  Please select a different node type, or add an taxonomy field to this node type.", 'error');
		}

		if ($choices['openlayers_cck']) {
			$form['flickrsync_openlayers_cck'] = array(
				'#type' => 'select',
				'#title' => t('Text field for attaching Geo Data '),
				'#default_value' => variable_get('flickrsync_openlayers_cck', ''),
				'#options' => $choices['openlayers_cck'],
				'#description' => t('Select the taxonomy field to use for attaching Flickr Geo Data to Drupal nodes.'),
			);
		}

		if ($choices['date']) {
			$form['flickrsync_date'] = array(
				'#type' => 'select',
				'#title' => t('Text field for attaching Date '),
				'#default_value' => variable_get('flickrsync_date', ''),
				'#options' => $choices['date'],
				'#description' => t('Select the taxonomy field to use for attaching Flickr date to Drupal nodes.'),
			);
		}
    $vocabs = taxonomy_get_vocabularies($type);
    $choices['image'] = array();
    foreach ($vocabs as $vocab) {
			// is free tagging enabled?
      if ($vocab->tags == 1) {
         $choices['image'][$vocab->vid] = $vocab->name;
      }
    }
    if ($choices['image']) {
      $form['flickrsync_vocab'] = array(
        '#type' => 'select',
        '#title' => t('Vocabulary for importing Flickr tags'),
        '#default_value' => variable_get('flickrsync_vocab', ''),
        '#options' => $choices['image'],
        '#description' => t('Select a free-tagging vocabulary to use for importing Flickr tags.'),
      );
      
    } else {
      drupal_set_message("The node type selected doesn't have a free tagging vocabulary.
        This is optional, but you want want to consider configuring a vocabulary so that Flickr tags
        can be imported.");
    }
  }
  $form['flickrsync_never_update_image'] = array(
    '#type' => 'checkbox',
    '#title' => t('Never Update Image'),
		'#default_value' => variable_get('flickrsync_never_update_image', 0),
  );
  $form['flickrsync_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Timeout when importing Flickr images'),
    '#default_value' => variable_get('flickrsync_timeout', 30),
    '#description' => t('The number of seconds to allow for an individual image import to complete.'),
  );
  $form['flickrsync_cron_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of Flickr images to import during each cron run'),
    '#default_value' => variable_get('flickrsync_cron_number', 10),
    '#description' => t('Use a low number to avoid PHP timing out during cron, especially if other intensive cron tasks are also running.  Keep in mind that you may need to run cron frequently if lots of Flickr photos need to be imported.'),
	);
  return system_settings_form($form);
}