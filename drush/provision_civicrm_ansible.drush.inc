<?php

/**
 * @file
 *   Aegir service autoloading function and switch based on version and include
 *   the appropriate files to handle install, verify, &c.
 */

# require_once('provision_civicrm_ansible.inc');
# require_once(dirname(__FILE__) . '/deploy.provision.inc');
# require_once(dirname(__FILE__) . '/install.provision.inc');
# require_once(dirname(__FILE__) . '/migrate.provision.inc');
require_once(dirname(__FILE__) . '/verify.provision.inc');

/**
 * Implements hook_drush_init().
 */
function provision_civicrm_ansible_drush_init() {
  // Register our service classes for autoloading.
  provision_civicrm_ansible_provision_register_autoload();
}

/**
 * Register our directory as a place to find Provision classes.
 *
 * This allows Provision to autoload our classes, so that we don't need to
 * specifically include the files before we use the class.
 */
function provision_civicrm_ansible_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    $list = drush_commandfile_list();
    $provision_dir = dirname($list['provision']);
    include_once($provision_dir . '/provision.inc');
    include_once($provision_dir . '/provision.service.inc');
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Helper function to execute and log the output of an Ansible command
 *
 * @param $site String Ansible site
 * @param $tag String Ansible tag
 */
function provision_civicrm_ansible_execute($site, $tag) {
  ob_start();
  passthru('sudo /usr/local/bin/aegir-ansible ' . escapeshellarg($site) . ' ' . escapeshellarg($tag));
  $output = ob_get_clean();
  $lines = explode("\n", $output);
  drush_log(dt('CiviCRM Ansible output...'), 'ok');
  foreach ($lines as $line) {
    $line = trim($line);
    $line = preg_replace('/ \*+$/', '', $line);
    if (!$line) {
      continue;
    }
    if (preg_match('/^(fail|error)/i', $line)) {
      drush_log($line, 'error');
    }
    elseif (preg_match('/warning/i', $line)) {
      drush_log($line, 'warning');
    }
    else {
      drush_log($line, 'ok');
    }
  }
}
