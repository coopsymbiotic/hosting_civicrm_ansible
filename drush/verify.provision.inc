<?php

/**
 * @file
 *
 * CiviCRM Ansible support module for the Aegir backend.
 *
 * This file contains the verify operations.
 */

/**
 * Implements hook_post_provision_verify().
 *
 * Run CiviCRM Ansible playbooks.
 */
function drush_provision_civicrm_ansible_post_provision_verify($url = NULL) {
  if (!_provision_civicrm_is_site_context(TRUE)) {
    return;
  }

  if (d()->type == 'site') {
    ob_start();
    passthru('sudo /usr/local/bin/aegir-ansible ' . escapeshellarg(d()->uri));
    $output = ob_get_clean();

    drush_log(dt("CiviCRM Ansible: !output", ['!output' => '<pre>' . $output . '</pre>']), 'ok');
  }
  elseif (d()->type == 'wpsite' && !function_exists('civicrm_initialize')) {
    drush_log(dt("CiviCRM Ansible: the CiviCRM plugin is not enabled. Skipping verify operations for CiviCRM. You will need to re-verify the site if you enable CiviCRM in the future."));
  }
}
