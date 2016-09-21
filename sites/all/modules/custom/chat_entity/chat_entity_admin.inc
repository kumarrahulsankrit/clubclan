<?php

/**
 * @file
 * Admin page callbacks for the comment module.
 */

/**
 * Menu callback; present an administrative comment listing.
 */
function comment_admin($type = 'new') {
    $edit = $_POST;

    if (isset($edit['operation']) && ($edit['operation'] == 'delete') && isset($edit['comments']) && $edit['comments']) {
        return drupal_get_form('comment_multiple_delete_confirm');
    }
    else {
        return drupal_get_form('comment_admin_overview', $type);
    }
}
